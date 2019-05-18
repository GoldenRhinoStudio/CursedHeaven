#include "j1Render.h"
#include "Wipe.h"
#include "j1Scene1.h"
#include "j1ChooseCharacter.h"
#include "j1App.h"
#include "j1Window.h"
#include "p2Log.h"

Wipe::Wipe(SCENE scene_to_remove, SCENE scene_to_appear, j1Color color, float time) : j1Transitions(time) {
	this->color = color;
	scene1 = scene_to_remove;
	scene2 = scene_to_appear;

	App->win->GetWindowSize(w, h);
	screen = { -(int)w, 0, (int)w + 5, (int)h };
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
}

Wipe::~Wipe()
{}

void Wipe::Start() {

	j1Transitions::Start();

	screen.x = (int)Interpolation(-(int)w, -2, percentage);

	SDL_SetRenderDrawColor(App->render->renderer, color.r, color.g, color.b, 255);
	SDL_RenderFillRect(App->render->renderer, &screen);
}

void Wipe::Change() {

	screen.x = screen.y = 0;

	SDL_SetRenderDrawColor(App->render->renderer, color.r, color.g, color.b, 255);
	SDL_RenderFillRect(App->render->renderer, &screen);

	App->transitions->SwitchScenes(scene1, scene2);

	j1Transitions::Change();
}

void Wipe::Exit() {

	j1Transitions::Exit();

	screen.x = (int)Interpolation(0, (int)w, percentage);

	SDL_SetRenderDrawColor(App->render->renderer, color.r, color.g, color.b, 255);
	SDL_RenderFillRect(App->render->renderer, &screen);

}