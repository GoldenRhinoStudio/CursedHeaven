#include "j1Scene1.h"
#include "j1TransitionManager.h"
#include "j1Transitions.h"
#include "j1Render.h"
#include "FadeToColor.h"
#include "j1App.h"
#include "j1Window.h"
#include "p2Log.h"

// Similar code to FadeToBlack, but now we can control the color easily

FadeToColor::FadeToColor(SCENE scene_to_remove, SCENE scene_to_appear, j1Color color, float time) : j1Transitions(time) {

	this->color = color;
	scene1 = scene_to_remove;
	scene2 = scene_to_appear;

	uint w, h;
	App->win->GetWindowSize(w, h);
	screen = { 0, 0, (int)w, (int)h };
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);

}

FadeToColor::~FadeToColor()
{}

void FadeToColor::Start() {

	// Filling the screen with a color
	FillScreen(Interpolation(0, 255, percentage));

	j1Transitions::Start();
}

void FadeToColor::Change() {

	FillScreen(255.0f);

	App->transitions->SwitchScenes(scene1, scene2);

	j1Transitions::Change();
}

void FadeToColor::Exit() {

	j1Transitions::Exit();

	// Color disappears
	FillScreen(Interpolation(255, 0, percentage));

}

void FadeToColor::FillScreen(float percentage) {

	if (percentage < 0)
		percentage = 0;

	if (percentage > 255)
		percentage = 255;

	SDL_SetRenderDrawColor(App->render->renderer, color.r, color.g, color.b, percentage);
	SDL_RenderFillRect(App->render->renderer, &screen);
}