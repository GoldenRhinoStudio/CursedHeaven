extern "C" {
#include "ffmpeg/include/libavcodec/avcodec.h"
#include "ffmpeg/include/libavdevice/avdevice.h"
#include "ffmpeg/include/libavfilter/avfilter.h"
#include "ffmpeg/include/libavformat/avformat.h"
#include "ffmpeg/include/libavutil/avutil.h"
#include "ffmpeg/include/libswscale/swscale.h"
#include "ffmpeg/include/libswresample/swresample.h"

#pragma comment( lib, "ffmpeg/lib/avcodec.lib" )
#pragma comment( lib, "ffmpeg/lib/avdevice.lib" )
#pragma comment( lib, "ffmpeg/lib/avfilter.lib" )
#pragma comment( lib, "ffmpeg/lib/avformat.lib" )
#pragma comment( lib, "ffmpeg/lib/avutil.lib" )
#pragma comment( lib, "ffmpeg/lib/swscale.lib" )
#pragma comment( lib, "ffmpeg/lib/swresample.lib" )
}

#include "j1App.h"
#include "j1Render.h"
#include "j1Input.h"
#include "j1Window.h"
#include "p2Log.h"
#include "j1Video.h"
#include "j1TransitionManager.h"
#include "j1SceneMenu.h"

#define DEFAULT_AUDIO_BUF_SIZE 1024
#define MAX_AUDIOQ_SIZE (5 * 256 * 1024)
#define MAX_VIDEOQ_SIZE (5 * 512 * 1024)

void AudioCallback(void *userdata, Uint8 *stream, int length) {

	j1Video *video = (j1Video*)userdata;
	int input_length, audio_size;

	while (length > 0 && video->playing) {
		if (video->audio_buf_index >= video->audio_buf_size) {
			// All audio data sent. Get more.
			audio_size = video->DecodeAudio();
			if (audio_size < 0) {
				// If an error occured decoding, we output silence (0)
				video->audio_buf_size = DEFAULT_AUDIO_BUF_SIZE;
				memset(video->audio_buf, 0, video->audio_buf_size);
			}
			else {
				video->audio_buf_size = audio_size;
			}
			video->audio_buf_index = 0;
		}
		input_length = video->audio_buf_size - video->audio_buf_index;
		if (input_length > length)
			input_length = length;
		memcpy(stream, (uint8_t *)video->audio_buf + video->audio_buf_index, input_length);
		length -= input_length;
		stream += input_length;
		video->audio_buf_index += input_length;
	}
}

int VideoCallback(Uint32 interval, void* param)
{
	j1Video* player = (j1Video*)param;
	if(player->playing)
		player->refresh = true;

	return 0;
}

int ReadThread(void *param) {
	j1Video* player = (j1Video*)param;
	AVPacket pkt;
	int ret = 0;

	while (player->playing)
	{
		if (player->audio.pktqueue.size >= MAX_AUDIOQ_SIZE 
			|| player->video.pktqueue.size >= MAX_VIDEOQ_SIZE)
		{
			SDL_Delay(10);
			continue;
		}

		// read an encoded packet from file
		ret = av_read_frame(player->format, &pkt);
		if (ret < 0)
		{
			if (ret == AVERROR_EOF) LOG("Finished reading packets");
			else LOG("Error reading packet");
			player->quit = true;
			break;
		}

		if (pkt.stream_index == player->video.stream_index) // if packet data is video we add to video queue
		{
			player->video.pktqueue.PutPacket(&pkt);
		}
		else if (pkt.stream_index == player->audio.stream_index)
		{
			player->audio.pktqueue.PutPacket(&pkt); // if packet data is audio we add to audio queue
		}
		av_packet_unref(&pkt); // release the packet
	}

	return 0;
}



j1Video::j1Video()
{
}


j1Video::~j1Video()
{
}

bool j1Video::Awake(pugi::xml_node&)
{
	return true;
}

bool j1Video::Start()
{
	PlayVideo("textures/intro.mp4");
	App->win->scale = 1;

	return true;
}

bool j1Video::PreUpdate()
{
	return true;
}

bool j1Video::Update(float dt)
{
	//Update texture
	if (refresh)
	{
		DecodeVideo();
		refresh = false;
	}
	if (quit && video.finished)
	{
		CloseVideo();
	}
	if (playing)
	{
		App->render->Blit(texture, 0, 0, nullptr);
	}
	
	return true;
}

bool j1Video::PostUpdate()
{

	return true;
}

bool j1Video::CleanUp()
{
	if(playing)
		CloseVideo();

	return true;
}

int j1Video::PlayVideo(std::string file_path)
{
	if (playing)
	{
		LOG("Already playing video");
		return -1;
	}

	file = file_path;

	//Open video file
	if (avformat_open_input(&format, file.c_str(), NULL, NULL) != 0)
	{
		LOG("Error loading video file %s", file);
		return -1;
	}

	// Retrieve stream information
	if (avformat_find_stream_info(format, NULL) <0)
		return -1; // Couldn't find stream information

	int video_stream = -1, audio_stream = -1;
	// Find video and audio streams
	for (int i = 0; i < format->nb_streams; i++)
	{
		if (format->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && video_stream < 0)
			video_stream = i;
		else if (format->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && audio_stream < 0)
			audio_stream = i;
	}

	if (video_stream == -1)
		LOG("No video stream found");
	else
		OpenStreamComponent(video_stream); //Open video component

	if (audio_stream == -1)
		LOG("No audio stream found");
	else
		OpenStreamComponent(audio_stream); //Open audio component

	//Everything went fine
	LOG("Video file loaded correctly, now playing: %s", file);
	playing = true;
	parse_thread_id = SDL_CreateThread(ReadThread, "ReadThread", this); //Start packet reading thread

	SDL_AddTimer(40, (SDL_TimerCallback)VideoCallback, this); //First video callback
}

void j1Video::OpenStreamComponent(int stream_index)
{
	AVCodecContext *codec_context;
	AVCodec *codec;

	if (stream_index < 0 || stream_index >= format->nb_streams) {
		LOG("Error not valid stream index");
	}

	// Find the decoder for stream
	codec = avcodec_find_decoder(format->streams[stream_index]->codecpar->codec_id);
	if (codec == NULL)
		LOG("Unsupported codec for stream index %i", stream_index);

	codec_context = avcodec_alloc_context3(codec);
	// Parameter to context for video
	if (avcodec_parameters_to_context(codec_context, format->streams[stream_index]->codecpar) < 0) {
		LOG("Failed parameters to context for stream %i", stream_index);
	}

	// Open codecs
	if (avcodec_open2(codec_context, codec, NULL) < 0)
	{
		LOG("Error opening codec");
	}

	switch (codec_context->codec_type)
	{
	case AVMEDIA_TYPE_VIDEO:
		video.stream = format->streams[stream_index];
		video.context = codec_context;
		video.stream_index = stream_index;

		video.frame = av_frame_alloc();
		video.converted_frame = av_frame_alloc();

		uint dst_w, dst_h;
		App->win->GetWindowSize(dst_w, dst_h);

		//Prepare scaling frame.
		video.converted_frame->format = AVPixelFormat::AV_PIX_FMT_YUV420P; // Format used for SDL
		video.converted_frame->width = dst_w; //width of window
		video.converted_frame->height = dst_h; //height of window
		av_frame_get_buffer(video.converted_frame, 0); //get buffers for YUV format

		//Get context to scale video frames to format, width and height of our SDL window.
		sws_context = sws_getContext(codec_context->width, codec_context->height, codec_context->pix_fmt,
			dst_w, dst_h, AVPixelFormat::AV_PIX_FMT_YUV420P,
			SWS_BILINEAR, NULL, NULL, NULL);

		//Create texture where we will output the video.
		texture = SDL_CreateTexture(App->render->renderer, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING,
			dst_w, dst_h);

		video.pktqueue.Init();

		break;
	case AVMEDIA_TYPE_AUDIO:
		SDL_AudioSpec wanted_spec, spec;

		audio.stream = format->streams[stream_index];
		audio.context = codec_context;
		audio.stream_index = stream_index;

		audio.frame = av_frame_alloc();
		audio.converted_frame = av_frame_alloc();

		//Fill wanted specs for sdl audio
		wanted_spec.freq = audio.context->sample_rate;
		wanted_spec.format = AUDIO_S16SYS;
		wanted_spec.channels = audio.context->channels;
		wanted_spec.silence = 0;
		wanted_spec.samples = DEFAULT_AUDIO_BUF_SIZE;
		wanted_spec.callback = AudioCallback;
		wanted_spec.userdata = this;

		if (SDL_OpenAudio(&wanted_spec, &spec) < 0) {
			LOG("SDL_OpenAudio: %s", SDL_GetError());
		}

		//Prepare conversion context
		swr_context = swr_alloc_set_opts(NULL, audio.context->channel_layout, AV_SAMPLE_FMT_FLT, audio.context->sample_rate,
			audio.context->channel_layout, audio.context->sample_fmt, audio.context->sample_rate, 0, NULL);
		swr_init(swr_context);

		//Get buffer to output converted audio
		audio.converted_frame->format = AV_SAMPLE_FMT_FLT; // Format used for SDL Audio
		audio.converted_frame->channel_layout = audio.context->channel_layout;
		audio.converted_frame->nb_samples = audio.context->frame_size;
		av_frame_get_buffer(audio.converted_frame, 0);

		audio.pktqueue.Init();
		SDL_PauseAudio(0);

		break;
	}
}

bool j1Video::Pause()
{
	if (!playing)
		return false;

	paused = !paused;

	SDL_PauseAudio(0);
	audio.pktqueue.paused = paused;
	video.pktqueue.paused = paused;

	if(paused) LOG("Video paused");
	else LOG("Video resumed");

	return true;
}

void j1Video::CloseVideo()
{
	if (!playing)
		return;

	playing = false;
	audio.pktqueue.paused = true;
	video.pktqueue.paused = true;

	SDL_CondSignal(video.pktqueue.cond); //In case queue is waiting for new pkts
	SDL_CondSignal(audio.pktqueue.cond); //In case queue is waiting for new pkts
	SDL_WaitThread(parse_thread_id, NULL);

	video.Clear();
	audio.Clear();

	sws_freeContext(sws_context);
	sws_context = nullptr;
	swr_free(&swr_context);

	avformat_close_input(&format);

	SDL_DestroyTexture(texture);
	texture = nullptr;

	quit = false;

	App->win->scale = 3;

	App->transitions->FadingToColor(VIDEO, MENU, White);
	
	LOG("Video closed");
}

void j1Video::DecodeVideo()
{
	AVPacket pkt;
	int ret;

	if (!playing)
		return;
	if (video.pktqueue.GetPacket(&pkt, quit) < 0)
	{
		if (quit)
			video.finished = true;
		else 
			SDL_AddTimer(1, (SDL_TimerCallback)VideoCallback, this);
		return;
	}


	//send packet to video decoder
	ret = avcodec_send_packet(video.context, &pkt);
	if (ret < 0)
	{
		LOG("Error sending video packet for decoding");
		return;
	}

	ret = avcodec_receive_frame(video.context, video.frame);

	//Scale texture
	sws_scale(sws_context, video.frame->data, video.frame->linesize, 
		0, video.frame->height, video.converted_frame->data, video.converted_frame->linesize);

	//Update video texture
	SDL_UpdateYUVTexture(texture, nullptr, video.converted_frame->data[0], video.converted_frame->linesize[0], video.converted_frame->data[1],
		video.converted_frame->linesize[1], video.converted_frame->data[2], video.converted_frame->linesize[2]);

	double pts = video.frame->pts;
	if (pts == AV_NOPTS_VALUE)
	{
		pts = video.clock +
			(1.f / av_q2d(video.stream->avg_frame_rate)) / av_q2d(video.stream->time_base);
	}
	video.clock = pts;
	double delay;
	if (audio.stream != nullptr){
		delay = (video.clock*av_q2d(video.stream->time_base)) - (audio.clock*av_q2d(audio.stream->time_base));
		if (delay < 0.01)
			delay = 0.01; //Maybe skip frame if video is too far behind from audio instead of fast refresh.
	}
	else {
		delay = (video.clock*av_q2d(video.stream->time_base));
		if (delay < 0.01)
			delay = 0.01; //Maybe skip frame if video is too far behind from audio instead of fast refresh.
	}

	//Prepare VideoCallback on ms
	SDL_AddTimer((Uint32)(delay * 50 + 0.5), (SDL_TimerCallback)VideoCallback, this);
	av_packet_unref(&pkt);
}

int j1Video::DecodeAudio()
{
	AVPacket pkt;
	int ret;
	int len2, data_size = 0;

	if (!playing)
		return -1;

	if (audio.pktqueue.GetPacket(&pkt, quit) < 0)
	{
		if (quit)
			audio.finished = true;
		return -1;
	}


	ret = avcodec_send_packet(audio.context, &pkt);
	if (ret < 0)
	{
		LOG("Error sending audio pkt to decode");
		return -1;
	}

	ret = avcodec_receive_frame(audio.context, audio.frame);

	swr_convert(swr_context,
		audio.converted_frame->data,	// output
		audio.frame->nb_samples,
		(const uint8_t**)audio.frame->data,  // input
		audio.frame->nb_samples);

	// returns the number of samples per channel in one audio frame
	data_size = av_samples_get_buffer_size(NULL,
		audio.context->channels,
		audio.frame->nb_samples,
		AV_SAMPLE_FMT_FLT,
		1);

	memcpy(audio_buf, audio.converted_frame->data[0], data_size);
	av_packet_unref(&pkt);

	// TODO 5.1 Just copy and paste, don't forget to change the variable names like I did
	// You dont need to cal SDL_Timer here if you were wondering
	double pts = audio.frame->pts;
	if (pts == AV_NOPTS_VALUE)
	{
		pts = audio.clock +
			(1.f / av_q2d(audio.stream->avg_frame_rate)) / av_q2d(audio.stream->time_base);
	}
	audio.clock = pts;

	// We have data, return it and come back for more later
	return data_size;
}

void PacketQueue::Init()
{
	paused = false;
	mutex = SDL_CreateMutex();
	cond = SDL_CreateCond();
}

int PacketQueue::PutPacket(AVPacket* pkt)
{
	AVPacketList *pkt_list;
	AVPacket new_pkt;

	if (paused)
		return -1;

	av_packet_ref(&new_pkt, pkt);

	pkt_list = (AVPacketList*)av_malloc(sizeof(AVPacketList));
	if (!pkt_list)
		return -1;

	pkt_list->pkt = new_pkt;
	pkt_list->next = nullptr;

	SDL_LockMutex(mutex);
	if (!last_pkt)
		first_pkt = pkt_list;
	else
		last_pkt->next = pkt_list;

	last_pkt = pkt_list;
	nb_packets++;
	size += pkt_list->pkt.size;

	SDL_CondSignal(cond);
	SDL_UnlockMutex(mutex);

	return 0;
}

int PacketQueue::GetPacket(AVPacket* pkt, bool block)
{
	AVPacketList *pkt_list;
	int ret;

	SDL_LockMutex(mutex);
	while (true)
	{
		if (paused) {
			ret = -1;
			break;
		}

		pkt_list = first_pkt;

		if (pkt_list) {
			first_pkt = pkt_list->next;
			if (!first_pkt)
				last_pkt = nullptr;
			nb_packets--;
			size -= pkt_list->pkt.size;
			*pkt = pkt_list->pkt;
			av_free(pkt_list);
			ret = 1;
			break;
		}
		else if (block)
		{
			ret = -1;
			break;
		}
		else {
			SDL_CondWait(cond, mutex); //Wait for pkt
		}
	}	
	SDL_UnlockMutex(mutex);
	return ret;
}

int PacketQueue::Clear()
{
	AVPacketList *pkt, *pkt1;
	SDL_LockMutex(mutex);
	for (pkt = first_pkt; pkt != nullptr; pkt = pkt1) {
		pkt1 = pkt->next;
		av_packet_unref(&(pkt->pkt));
		av_freep(&pkt);
	}
	last_pkt = nullptr;
	first_pkt = nullptr;
	nb_packets = 0;
	size = 0;
	SDL_UnlockMutex(mutex);

	SDL_DestroyMutex(mutex);
	SDL_DestroyCond(cond);

	return 0;
}

void StreamComponent::Clear()
{
	pktqueue.Clear();

	av_frame_free(&frame);
	av_frame_free(&converted_frame);

	avcodec_free_context(&context);

	clock = 0;
	stream_index = -1;
	finished = false;

}
