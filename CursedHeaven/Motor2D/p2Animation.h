#ifndef __P2ANIMATION_H__
#define __P2ANIMATION_H__

#include "SDL/include/SDL_rect.h"
#include <string>
#define MAX_FRAMES 100

class Animation
{
public:
	bool loop = true;
	float speed = 1.0f;
	SDL_Rect frames[MAX_FRAMES];

private:
	float current_frame;
	int last_frame = 0;
	int loops = 0;

	pugi::xml_document	animations_file;

public:

	Animation::Animation() {}

	Animation(const Animation& anim) : loop(anim.loop), speed(anim.speed), last_frame(anim.last_frame)
	{
		SDL_memcpy(&frames, anim.frames, sizeof(frames));
	}

	void PushBack(const SDL_Rect& rect)
	{
		frames[last_frame++] = rect;
	}

	void LoadAnimation(std::string name, std::string entity, bool player = true)
	{
		pugi::xml_parse_result result = animations_file.load_file("animations.xml");
		if (result != NULL)
		{
			pugi::xml_node animation_name;

			if (player)
				animation_name = animations_file.child("animations").child("player").child(entity.data()).child(name.data());
			else
				animation_name = animations_file.child("animations").child("enemies").child(entity.data()).child(name.data());

			loop = animation_name.attribute("loop").as_bool();
			speed = animation_name.attribute("speed").as_float();
			for (pugi::xml_node animation = animation_name.child("animation"); animation; animation = animation.next_sibling("animation"))
			{
				PushBack({ animation.attribute("x").as_int(), animation.attribute("y").as_int(), animation.attribute("w").as_int(), animation.attribute("h").as_int() });
			}
		}
	}

	SDL_Rect& GetCurrentFrame(float dt)
	{
		if (this)
		{
			current_frame += speed * dt;
			if (current_frame >= last_frame)
			{
				current_frame = (loop) ? 0.0f : last_frame - 1;
				loops++;
			}
			return frames[(int)current_frame];
		}
	}

	bool Finished()
	{
		if (loops > 0) {
			return true;
		}
		else
			return false;
	}

	void Reset()
	{
		current_frame = 0;
		loops = 0;
	}
};

#endif // __P2ANIMATION_H__
