#include "j1DialogSystem.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Window.h"
#include "j1Render.h"
#include "j1Scene1.h"
#include "j1Entity.h"
#include "j1Label.h"
#include "j1Fonts.h"
#include <time.h>

j1DialogSystem::j1DialogSystem()
{
	srand(time(NULL));
}

j1DialogSystem::~j1DialogSystem()
{
}


bool j1DialogSystem::Start() {

	bool ret = true;

	dialogFont = App->font->Load("fonts/Pixeled.ttf", 5);
	dialog_tex = App->tex->Load("textures/dialog_final.png");
	dialog_tex2 = App->tex->Load("textures/dialog_final.png");
	dialog_tex3 = App->tex->Load("textures/dialog_final.png");

	law = (1 + rand() % 2);

	if (law == 1) {

		law1Active = true;
		LOG("law1 act");
	}

	else if (law == 2) {

		law2Active = true;
		LOG("law2 act");
	}

	return ret;
};


bool j1DialogSystem::Update(float dt) {

	//Rects
	SDL_Rect chart1 = { 0, 0, 284, 71 };
	SDL_Rect chartoption1 = { 0, 142, 220, 54 };
	SDL_Rect chartoption2 = { 0, 196, 220, 54 };
	SDL_Rect chart2 = { 0, 71, 284, 71 };

	if (law1Active == true)
	{
		App->render->Blit(dialog_tex, 0, 20, &chart1, SDL_FLIP_NONE, 1.0f, 1.0f, 0.0, pivot, pivot, false);

		if (App->input->GetMouseButtonDown(1) == KEY_DOWN) {
			times += 1;
			LOG("times: %d", times);
		}
		
		if (times == 1) {
			App->tex->UnLoad(dialog_tex);
			App->render->Blit(dialog_tex2, 400, 550, &chartoption1, SDL_FLIP_NONE, 1.0f, 1.0f, 0.0, pivot, pivot, false);
		}

		if (times == 2) {
			App->tex->UnLoad(dialog_tex2);
			App->render->Blit(dialog_tex3, 0, 20, &chart2, SDL_FLIP_NONE, 1.0f, 1.0f, 0.0, pivot, pivot, false);
		}
		
		if (times == 3) {
			App->tex->UnLoad(dialog_tex3);
			times = 0;
			App->scene1->finishedDialog = true;
			law1Active = false;
		}
	}
	else if (law2Active == true)
	{
		App->render->Blit(dialog_tex, 0, 20, &chart1, SDL_FLIP_NONE, 1.0f, 1.0f, 0.0, pivot, pivot, false);

		if (App->input->GetMouseButtonDown(1) == KEY_DOWN) {
			times += 1;
			LOG("times: %d", times);
		}

		if (times == 1) {
			App->tex->UnLoad(dialog_tex);
			App->render->Blit(dialog_tex2, 400, 550, &chartoption2, SDL_FLIP_NONE, 1.0f, 1.0f, 0.0, pivot, pivot, false);
		}

		if (times == 2) {
			App->tex->UnLoad(dialog_tex2);
			App->render->Blit(dialog_tex3, 0, 20, &chart2, SDL_FLIP_NONE, 1.0f, 1.0f, 0.0, pivot, pivot, false);
		}

		if (times == 3) {
			App->tex->UnLoad(dialog_tex3);
			times = 0;
			App->scene1->finishedDialog = true;
			law2Active = false;
		}
	}

	return true;
}


bool j1DialogSystem::CleanUp() {

	App->tex->UnLoad(dialog_tex);
	App->tex->UnLoad(dialog_tex2);
	App->tex->UnLoad(dialog_tex3);

	return true;
};