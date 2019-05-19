#include "j1DialogSystem.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Window.h"
#include "j1Render.h"
#include "j1Scene1.h"
#include "j1Scene2.h"
#include "j1Entity.h"
#include "j1Label.h"
#include "j1Box.h"
#include "j1Fonts.h"
#include "j1DragoonKnight.h"
#include "j1BlackMage.h"
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

	judge_tex = App->tex->Load("textures/character/judge/Judge.png");
	dialog_tex = App->tex->Load("textures/dialog_final.png");
	dialog_tex2 = App->tex->Load("textures/dialog_final.png");
	dialog_tex3 = App->tex->Load("textures/dialog_final.png");
	seller_tex1 = App->tex->Load("textures/dialog_final.png");
	seller_tex2 = App->tex->Load("textures/dialog_final.png");

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
	SDL_Rect chartoption1 = { 277, 0, 221, 54 };
	SDL_Rect chartoption2 = { 277, 54, 220, 54 };

	SDL_Rect chart1s1 = { 0, 0, 277, 61 };
	SDL_Rect chart2s1 = { 0, 61, 276, 61 };

	SDL_Rect chart1s2 = { 0, 122, 277, 61 };
	SDL_Rect chart2s2 = { 0, 183, 277, 61 };

	SDL_Rect chart3s2 = { 0, 244, 277, 61 };
	SDL_Rect chart4s2 = { 277, 108, 279, 61 };

	SDL_Rect judgeRect = { 42, 2, 22, 33 };

	if (App->scene1->active) {

		// judge sprite
		App->render->BlitHUD(judge_tex, 210, 750, &judgeRect, SDL_FLIP_HORIZONTAL, 1.0f, 1.0f, 0.0, pivot, pivot, true);
		if (!App->scene1->finishedDialog) App->gamePaused = true;

		if (!timerStarted) {
			dialogTimer.Start();
			time_passed = dialogTimer.Read();
			timerStarted = true;
		}

		// Judge 1st Dialog
		if (law1Active == true)
		{
			if(dialogTimer.Read() >= time_passed + dialogTime)
				App->render->BlitHUD(dialog_tex, 0, 20, &chart1s1, SDL_FLIP_NONE, 1.0f, 1.0f, 0.0, pivot, pivot, false);

			if (dialogTimer.Read() >= time_passed + dialogTime) {
				canSkip = true;
			}

			if ((App->input->GetMouseButtonDown(1) == KEY_DOWN || SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_A) == KEY_DOWN)
				&& canSkip && App->scene1->settings_window->visible == false) {
				times++;
				canSkip = false;
				time_passed = dialogTimer.Read();
				LOG("times: %d", times);
			}
		
			if (times == 1) {
				App->tex->UnLoad(dialog_tex);
				App->render->BlitHUD(dialog_tex2, 400, 550, &chartoption1, SDL_FLIP_NONE, 1.0f, 1.0f, 0.0, pivot, pivot, false);
			}

			if (times == 2) {
				App->tex->UnLoad(dialog_tex2);
				App->render->BlitHUD(dialog_tex3, 0, 20, &chart2s1, SDL_FLIP_NONE, 1.0f, 1.0f, 0.0, pivot, pivot, false);
			}
		
			if (times == 3) {
				App->tex->UnLoad(dialog_tex3);
				times = 0;
				App->scene1->finishedDialog = true;
				App->gamePaused = false;
				law1Active = false;
			}
		}
		else if (law2Active == true)
		{			
			if (dialogTimer.Read() >= time_passed + dialogTime) {
				App->render->BlitHUD(dialog_tex, 0, 20, &chart1s1, SDL_FLIP_NONE, 1.0f, 1.0f, 0.0, pivot, pivot, false);
				canSkip = true;
			}

			if ((App->input->GetMouseButtonDown(1) == KEY_DOWN || SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_A) == KEY_DOWN) 
				&& canSkip && App->scene1->settings_window->visible == false) {
				times++;
				canSkip = false;
				time_passed = dialogTimer.Read();
				LOG("times: %d", times);
			}

			if (times == 1) {
				App->tex->UnLoad(dialog_tex);
				App->render->BlitHUD(dialog_tex2, 400, 550, &chartoption2, SDL_FLIP_NONE, 1.0f, 1.0f, 0.0, pivot, pivot, false);
			}

			if (times == 2) {
				App->tex->UnLoad(dialog_tex2);
				App->render->BlitHUD(dialog_tex3, 0, 20, &chart2s1, SDL_FLIP_NONE, 1.0f, 1.0f, 0.0, pivot, pivot, false);
			}

			if (times == 3) {
				App->tex->UnLoad(dialog_tex3);
				times = 0;
				App->scene1->finishedDialog = true;
				App->gamePaused = false;
				law2Active = false;
			}
		}

		// Seller Dialog
		if ((App->entity->currentPlayer->position.x >= -1064 && App->entity->currentPlayer->position.x <= -984) && (App->entity->currentPlayer->position.y >= 652 && App->entity->currentPlayer->position.y <= 691))
		{
			if (App->scene1->dialogSellerCanAppear) {

				App->scene1->finishedDialog = false;

				if (!timerStartedSeller) {
					sellerTimer.Start();
					time_passed_seller = sellerTimer.Read();
					timerStartedSeller = true;
				}

				if (sellerTimer.Read() >= time_passed_seller + dialogTime)
					/*App->render->BlitHUD(dialog_tex, 0, 20, &chart1s1, SDL_FLIP_NONE, 1.0f, 1.0f, 0.0, pivot, pivot, false);*/

					if (sellerTimer.Read() >= time_passed_seller + dialogTime) {
						canSkip = true;
					}

				if ((App->input->GetMouseButtonDown(1) == KEY_DOWN || SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_A) == KEY_DOWN)
					&& canSkip && App->scene1->settings_window->visible == false) {
					times++;
					canSkip = false;
					time_passed_seller = sellerTimer.Read();
					LOG("times: %d", times);
				}

				if (times == 1) {

					/*App->tex->UnLoad(dialog_tex);
					App->render->BlitHUD(dialog_tex2, 400, 550, &chartoption1, SDL_FLIP_NONE, 1.0f, 1.0f, 0.0, pivot, pivot, false);*/
				}

				if (times == 2) {
					/*App->tex->UnLoad(dialog_tex2);
					App->render->BlitHUD(dialog_tex3, 0, 20, &chart2s1, SDL_FLIP_NONE, 1.0f, 1.0f, 0.0, pivot, pivot, false);*/
				}

				if (times == 3) {
					/*App->tex->UnLoad(dialog_tex3);*/
					times = 0;
					App->scene1->finishedDialog = true;
					App->scene1->dialogSellerCanAppear = false;
					App->gamePaused = false;

				}
			}
		}
	}

	else if (App->scene2->active) 
	{
		// judge sprite
		App->render->BlitHUD(judge_tex, 180, 750, &judgeRect, SDL_FLIP_HORIZONTAL, 1.0f, 1.0f, 0.0, pivot, pivot, true);
		if (!App->scene1->finishedDialog) App->gamePaused = true;

		if (!timerStarted2) {
			dialogTimer2.Start();
			time_passed_2 = dialogTimer2.Read();
			timerStarted2 = true;
		}

		// Judge 2st Dialog
		if (law1Active == true)
		{
			if (dialogTimer2.Read() >= time_passed_2 + dialogTime)
				App->render->BlitHUD(dialog_tex, 0, 20, &chart1s1, SDL_FLIP_NONE, 1.0f, 1.0f, 0.0, pivot, pivot, false);

			if (dialogTimer2.Read() >= time_passed_2 + dialogTime) {
				canSkip = true;
			}

			if ((App->input->GetMouseButtonDown(1) == KEY_DOWN || SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_A) == KEY_DOWN)
				&& canSkip/* && App->scene1->settings_window->visible == false*/) {
				times++;
				canSkip = false;
				time_passed_2 = dialogTimer2.Read();
				LOG("times: %d", times);
			}

			if (times == 1) {
				App->tex->UnLoad(dialog_tex);
				App->render->BlitHUD(dialog_tex2, 400, 550, &chartoption1, SDL_FLIP_NONE, 1.0f, 1.0f, 0.0, pivot, pivot, false);
			}

			if (times == 2) {
				App->tex->UnLoad(dialog_tex2);
				App->render->BlitHUD(dialog_tex3, 0, 20, &chart2s1, SDL_FLIP_NONE, 1.0f, 1.0f, 0.0, pivot, pivot, false);
			}

			if (times == 3) {
				App->tex->UnLoad(dialog_tex3);
				times = 0;
				App->scene1->finishedDialog = true;
				App->gamePaused = false;
				law1Active = false;
			}
		}
		else if (law2Active == true)
		{
			if (dialogTimer2.Read() >= time_passed_2 + dialogTime) {
				/*App->render->BlitHUD(dialog_tex, 0, 20, &chart1s1, SDL_FLIP_NONE, 1.0f, 1.0f, 0.0, pivot, pivot, false);*/
				canSkip = true;
			}

			if ((App->input->GetMouseButtonDown(1) == KEY_DOWN || SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_A) == KEY_DOWN)
				&& canSkip /*&& App->scene1->settings_window->visible == false*/) {
				times++;
				canSkip = false;
				time_passed_2 = dialogTimer2.Read();
				LOG("times: %d", times);
			}

			if (times == 1) {
				/*App->tex->UnLoad(dialog_tex);
				App->render->BlitHUD(dialog_tex2, 400, 550, &chartoption2, SDL_FLIP_NONE, 1.0f, 1.0f, 0.0, pivot, pivot, false);*/
			}

			if (times == 2) {
				/*App->tex->UnLoad(dialog_tex2);
				App->render->BlitHUD(dialog_tex3, 0, 20, &chart2s1, SDL_FLIP_NONE, 1.0f, 1.0f, 0.0, pivot, pivot, false);*/
			}

			if (times == 3) {
				/*App->tex->UnLoad(dialog_tex3);*/
				times = 0;
				App->scene1->finishedDialog = true;
				App->gamePaused = false;
				law2Active = false;
			}
		}
	}

	return true;
}

bool j1DialogSystem::CleanUp() {

	App->tex->UnLoad(dialog_tex);
	App->tex->UnLoad(dialog_tex2);
	App->tex->UnLoad(dialog_tex3);
	App->tex->UnLoad(judge_tex);

	return true;
};