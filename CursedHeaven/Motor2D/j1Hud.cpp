#include "j1Hud.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1UserInterfaceElement.h"
#include "j1Shop.h"
#include "j1Label.h"
#include "j1Scene1.h"
#include "j1Fonts.h"
#include "j1Render.h"
#include "j1Player.h"
#include "j1Input.h"
#include "j1Window.h"

j1Hud::j1Hud() 
{
	
}

j1Hud::~j1Hud() {}

bool j1Hud::Start()
{
	hud_text = App->tex->Load("gui/hud.png");
	profile_text = App->tex->Load("gui/player_profile.png");
	life_points = 82;
	multiplier = 82 / (float)App->entity->currentPlayer->lifePoints;

	// Potions
	potionsLabel = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, 15, 170, App->gui->font3, "x0", App->gui->brown);
	
	return true;
}

bool j1Hud::Update(float dt)
{
	App->gui->UpdateButtonsState(&hud_buttons, App->gui->buttonsScale);

	life_points = App->entity->currentPlayer->lifePoints * multiplier;

	for (std::list<j1Button*>::iterator item = hud_buttons.begin(); item != hud_buttons.end(); ++item) {
		if ((*item)->visible) {
			switch ((*item)->state) {
			case IDLE:
				(*item)->situation = (*item)->idle;
				break;

			case HOVERED:
				(*item)->situation = (*item)->hovered;
				break;

			case RELEASED:
				(*item)->situation = (*item)->idle;
				break;

			case CLICKED:
				(*item)->situation = (*item)->clicked;
				break;
			}
		}
	}

	// Profiles
	SDL_Rect dk_profile = { 0,0,42,42 };
	SDL_Rect bm_profile = { 42,0,43,42 };

	// Tab clicked
	SDL_Rect blackMage = { 0,568,190,366 };
	SDL_Rect dragoonKnight = { 190,568,273,361 };
	SDL_Rect window_profile_1 = { 0,0,785,568 };
	SDL_Rect window_profile_2 = { 785,0,785,568 };
	SDL_Rect ability = { 561,587,28,28 };

	// Coins
	SDL_Rect coins_r = { 0,42,35,13 };
	
	// DK Q
	SDL_Rect dk_available_q = {0,61,41,41};
	SDL_Rect dk_notavailable_q = {0,102,41,41};
	// DK E
	SDL_Rect dk_available_e = { 0,143,41,41 };
	SDL_Rect dk_notavailable_e = {0,184,41,41};

	// BM Q
	SDL_Rect bm_available_q = { 41,61,41,41 };
	SDL_Rect bm_notavailable_q = { 41,102,41,41 };
	// BM E
	SDL_Rect bm_available_e = { 41,143,41,41 };
	SDL_Rect bm_notavailable_e = { 41,184,41,41 };

	// Lifebar
	SDL_Rect lifebar = { 0,225,82,8 };
	SDL_Rect lifebar_r = { 0,233, life_points, 6 };

	if (App->entity->player_type == MAGE) {
		// Icon profile
		App->render->Blit(hud_text, 5, 5, &bm_profile, SDL_FLIP_NONE, false, 1.0f);

		// Abilities
		// Q
		App->render->Blit(hud_text, 15, 300, &bm_available_q, SDL_FLIP_NONE, false, 0.5f);
		if (!App->entity->currentPlayer->available_Q) {
			App->render->Blit(hud_text, 15, 300, &bm_notavailable_q, SDL_FLIP_NONE, false, 0.5f);
		}
		// E
		App->render->Blit(hud_text, 15, 400, &bm_available_e, SDL_FLIP_NONE, false, 0.5f);
		if (!App->entity->currentPlayer->available_E) {
			App->render->Blit(hud_text, 15, 400, &bm_notavailable_e, SDL_FLIP_NONE, false, 0.5f);
		}

	}
	else if (App->entity->player_type == KNIGHT) {
		// Icon profile
		App->render->Blit(hud_text, 5, 5, &dk_profile, SDL_FLIP_NONE, false);

		// Abilities
		// Q
		App->render->Blit(hud_text, 15, 300, &dk_available_q, SDL_FLIP_NONE, false, 0.5f);
		if (!App->entity->currentPlayer->available_Q) {
			App->render->Blit(hud_text, 15, 300, &dk_notavailable_q, SDL_FLIP_NONE, false, 0.5f);
		}
		// E
		App->render->Blit(hud_text, 15, 400, &dk_available_e, SDL_FLIP_NONE, false, 0.5f);
		if (!App->entity->currentPlayer->available_E) {
			App->render->Blit(hud_text, 15, 400, &dk_notavailable_e, SDL_FLIP_NONE, false, 0.5f);
		}
	}

	/*if (App->shop->potions == 0) {
		delete potionsLabel;
		potionsLabel = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, 15, 170, App->gui->font3, "x0", App->gui->brown);
	}
	else if (App->shop->potions == 1) {
		delete potionsLabel;
		potionsLabel = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, 15, 170, App->gui->font3, "x1", App->gui->brown);
	}
	else if (App->shop->potions == 2) {
		delete potionsLabel;
		potionsLabel = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, 15, 170, App->gui->font3, "x2", App->gui->brown);
	}
	else if (App->shop->potions == 3) {
		delete potionsLabel;
		potionsLabel = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, 15, 170, App->gui->font3, "x3", App->gui->brown);
	};*/

	potionsLabel->Draw(0.5, 0, 0, false);

	// Current points of the player (char*)
	current_points = App->scene1->current_points.c_str();
	//App->scene1->current_points.erase();
	SDL_Rect temp;
	temp.x = temp.y = 0;
	temp.w = temp.h = 10;

	App->render->Blit(hud_text, 16, 135, &coins_r, SDL_FLIP_NONE, false);

	App->tex->UnLoad(score);
	score = App->font->Print(current_points, temp.w, temp.h, 0, App->gui->brown, App->gui->font3);

	App->render->BlitDialog(score, 65, 130, &temp, SDL_FLIP_NONE, 0);

	App->render->Blit(hud_text, 140, 10, &lifebar, SDL_FLIP_NONE, false);
	App->render->Blit(hud_text, 143, 13, &lifebar_r, SDL_FLIP_NONE, false);

	for (std::list<j1Button*>::iterator item = hud_buttons.begin(); item != hud_buttons.end(); ++item) {
		(*item)->Draw(App->gui->buttonsScale);
	}
	for (std::list<j1Label*>::iterator item = labels_list.begin(); item != labels_list.end(); ++item) {
		(*item)->Draw(1.0f, 0, 0, false);
	}

	if (App->scene1->profile_active) {
		if (App->entity->player_type == MAGE) {
			App->render->Blit(profile_text, 150, 100, &window_profile_1, SDL_FLIP_NONE, false, 0.3f);
			App->render->Blit(profile_text, 185, 220, &blackMage, SDL_FLIP_NONE, false, 0.3f);
		}
		else if (App->entity->player_type == KNIGHT) {
			App->render->Blit(profile_text, 150, 100, &window_profile_2, SDL_FLIP_NONE, false, 0.3f);
			App->render->Blit(profile_text, 158, 220, &dragoonKnight, SDL_FLIP_NONE, false, 0.3f);
		}
	}

	return true;
}

bool j1Hud::CleanUp()
{
	App->tex->UnLoad(hud_text);
	App->tex->UnLoad(profile_text);
	App->tex->UnLoad(score);

	for (std::list<j1Button*>::iterator item = hud_buttons.begin(); item != hud_buttons.end(); ++item) {
		(*item)->CleanUp();
		hud_buttons.remove(*item);
	}
	for (std::list<j1Label*>::iterator item = labels_list.begin(); item != labels_list.end(); ++item) {
		(*item)->CleanUp();
		labels_list.remove(*item);
	}

	return true;
}

bool j1Hud::Load(pugi::xml_node & data)
{
	return true;
}

bool j1Hud::Save(pugi::xml_node & data) const
{
	return true;
}
