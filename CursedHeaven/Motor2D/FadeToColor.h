#pragma once

#include "j1TransitionManager.h"
#include "j1Transitions.h"

class FadeToColor : public j1Transitions {
public:

	FadeToColor(SCENE scene_to_remove, SCENE scene_to_appear, j1Color color = Black, float time = 2.0f);
	~FadeToColor();

	void Start();
	void Change();
	void Exit();

	void FillScreen(float percentage);

private:

	j1Color color;
	SDL_Rect screen;
	SCENE scene1, scene2;
	bool changing_scene = false;
};