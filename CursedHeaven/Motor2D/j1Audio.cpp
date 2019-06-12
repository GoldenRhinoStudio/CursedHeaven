#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Audio.h"

#include "SDL/include/SDL.h"
#include "SDL_mixer\include\SDL_mixer.h"
#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

#define DEFAULT_VOLUME MIX_MAX_VOLUME/2

j1Audio::j1Audio() : j1Module()
{
	music = NULL;
	name.assign("audio");

	currentfxvolume = DEFAULT_VOLUME;
	currentmusicvolume = DEFAULT_VOLUME;
}

// Destructor
j1Audio::~j1Audio()
{}

// Called before render is available
bool j1Audio::Awake(pugi::xml_node& config)
{
	LOG("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		active = false;
		ret = true;
	}

	// load support for the JPG and PNG image formats
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if((init & flags) != flags)
	{
		LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		active = false;
		ret = true;
	}

	//Initialize SDL_mixer
	if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		active = false;
		ret = true;
	}

	MusicVolume(DEFAULT_VOLUME);
	FxVolume(DEFAULT_VOLUME);

	slime_damage = LoadFx("audio/fx/Enemy_damage.wav");
	boss_damage = LoadFx("audio/fx/song139.wav");
	dash = LoadFx("audio/fx/song133.wav");
	explosion = LoadFx("audio/fx/song402.wav");
	rage_bm = LoadFx("audio/fx/song154.wav");
	rage_dk = LoadFx("audio/fx/song383.wav");
	attack_bm = LoadFx("audio/fx/song105.wav");
	attack_dk = LoadFx("audio/fx/song346.wav");
	death_bm = LoadFx("audio/fx/song508.wav");
	death_dk = LoadFx("audio/fx/song295.wav");
	damage_bm = LoadFx("audio/fx/song117.wav");
	damage_dk = LoadFx("audio/fx/song509.wav");
	slime_death = LoadFx("audio/fx/song122.wav");
	boss_death = LoadFx("audio/fx/song501.wav");
	slime_attack = LoadFx("audio/fx/song125.wav");
	boss_attack = LoadFx("audio/fx/song431.wav");
	coin_sound = LoadFx("audio/fx/coin.wav");
	potion_sound = LoadFx("audio/fx/potion.wav");
	heal_sound = LoadFx("audio/fx/heal.wav");
	shield_sound = LoadFx("audio/fx/Shield.wav");
	change_key_sound = LoadFx("audio/fx/ControlChange.wav");

	return ret;
}

// Called before quitting
bool j1Audio::CleanUp()
{
	if(!active)
		return true;

	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	if(music != NULL)
	{
		Mix_FreeMusic(music);
	}

	for (std::list<Mix_Chunk*>::iterator item = fx.begin(); item != fx.end(); ++item)
	{
		Mix_FreeChunk(*item);
	}

	fx.clear();

	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	return true;
}

// Play a music file
bool j1Audio::PlayMusic(const char* path, float fade_time)
{
	bool ret = true;

	if(!active)
		return false;

	if(music != NULL)
	{
		if(fade_time > 0.0f)
		{
			Mix_FadeOutMusic(int(fade_time * 1000.0f));
		}
		else
		{
			Mix_HaltMusic();
		}

		// this call blocks until fade out is done
		//Mix_FreeMusic(music);
	}

	music = Mix_LoadMUS(path);

	if(music == NULL)
	{
		LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if(fade_time > 0.0f)
		{
			if(Mix_FadeInMusic(music, -1, (int) (fade_time * 1000.0f)) < 0)
			{
				LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{
			if(Mix_PlayMusic(music, -1) < 0)
			{
				LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
	}

	LOG("Successfully playing %s", path);
	return ret;
}

// Load WAV
unsigned int j1Audio::LoadFx(const char* path)
{
	unsigned int ret = 0;

	if(!active)
		return 0;

	Mix_Chunk* chunk = Mix_LoadWAV(path);

	if(chunk == NULL)
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		Mix_VolumeChunk(chunk, currentfxvolume);
		fx.push_back(chunk);
		ret = fx.size();
	}

	return ret;
}

void j1Audio::UnloadFx() {
	std::list<Mix_Chunk*>::iterator* item;

	for (std::list<Mix_Chunk*>::iterator item = fx.begin(); item != fx.end(); ++item)
	{
		Mix_FreeChunk(*item);
	}
	fx.clear();
}

// Play WAV
bool j1Audio::PlayFx(unsigned int id, int repeat)
{
	bool ret = false;

	if(!active)
		return false;

	if(id > 0 && id <= fx.size())
	{
		std::list<Mix_Chunk*>::const_iterator item;
		item = next(fx.begin(), id - 1);
		Mix_PlayChannel(-1, *item, repeat);
	}

	return ret;
}

void j1Audio::MusicVolume(float vol)
{
	if (vol > MIX_MAX_VOLUME) 
		vol = MIX_MAX_VOLUME; 
	
	else if (vol < -MIX_MAX_VOLUME)
		vol = -MIX_MAX_VOLUME;

	Mix_VolumeMusic(vol);
	currentmusicvolume = vol;
}

void j1Audio::FxVolume(float vol)
{
	if (vol > MIX_MAX_VOLUME) 
		vol = MIX_MAX_VOLUME; 

	for (std::list<Mix_Chunk*>::iterator item = fx.begin(); item != fx.end(); ++item)
	{
		Mix_VolumeChunk(*item, vol);
	}
	currentfxvolume = vol;
}

float j1Audio::GetFxVolume()  
{
	return currentfxvolume;
}

float j1Audio::GetMusicVolume()  
{
	return currentmusicvolume;
}
