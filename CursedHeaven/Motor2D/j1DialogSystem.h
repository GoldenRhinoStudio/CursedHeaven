
#ifndef __j1DIALOGSYSTEM_H__
#define __j1DIALOGSYSTEM_H__

#include <iostream> 
#include <sstream>
#include <vector>
#include "j1App.h"
#include "j1Module.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Gui.h"
#include "j1Label.h"
#include "SDL/include/SDL.h"
#include <list>

class j1Label;

struct _TTF_Font;
struct SDL_Texture;

class j1DialogSystem : public j1Module
{
public:
	j1DialogSystem();
	~j1DialogSystem();

	bool Start();
	bool Update(float dt);
	bool CleanUp();

public:

	int law = 0;
	SDL_Texture* judge_tex = nullptr;
	SDL_Texture* dialog_tex = nullptr;
	SDL_Texture* dialog_tex2 = nullptr;
	SDL_Texture* dialog_tex3 = nullptr;

	std::list<j1Label*> text_list;
	j1Timer dialogTimer;
	uint time_passed = 0;

	bool law1Active = false;
	bool law2Active = false;
	bool law3Active = false;
	bool canSkip = false;

	int times = 0;
	int pivot = 2147483647;

private:
	bool timerStarted = false;
	uint dialogTime = 1000;
};

#endif 

// __j1DIALOGSYSTEM_H__