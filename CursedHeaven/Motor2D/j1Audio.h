#ifndef __j1AUDIO_H__
#define __j1AUDIO_H__

#include "j1Module.h"
#include <list>

#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;
struct Mix_Chunk;

class j1Audio : public j1Module
{
public:

	j1Audio();

	// Destructor
	virtual ~j1Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

	void MusicVolume(float vol);
	void FxVolume(float vol);

	float GetMusicVolume();
	float GetFxVolume();

	int slime_damage, boss_damage;
	int dash, explosion, rage_bm, rage_dk;
	int attack_bm, death_bm, damage_bm;
	int attack_dk, death_dk, damage_dk;
	int slime_death, boss_death, slime_attack, boss_attack;

private:

	float currentmusicvolume;
	float currentfxvolume;

	_Mix_Music*			music = NULL;
	std::list<Mix_Chunk*>	fx;
};

#endif // __j1AUDIO_H__