#include "j1SceneSettings.h"
#include "j1SceneKeyConfig.h"
#include "j1SceneMenu.h"
#include "j1SceneCredits.h"
#include "j1App.h"
#include "p2Log.h"
#include "j1Textures.h"
#include "j1FadeToBlack.h"
#include "j1Audio.h"
#include "j1Map.h"
#include "j1Gui.h"
#include "j1Button.h"
#include "j1Label.h"
#include "j1Box.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Window.h"
#include "j1TransitionManager.h"

#include "Brofiler/Brofiler.h"

j1SceneKeyConfig::j1SceneKeyConfig()
{}

j1SceneKeyConfig::~j1SceneKeyConfig()
{}

bool j1SceneKeyConfig::Awake(pugi::xml_node &) {
	LOG("Loading Key Config");

	bool ret = true;

	if (App->menu->active) active = false;

	if (!active) LOG("Key Config Scene not active.");

	return ret;
}

bool j1SceneKeyConfig::Start() {

	if (active) {

		App->map->draw_with_quadtrees = false;
		App->map->Load("menu.tmx");
		
		key_tex = App->tex->Load("gui/uipack_rpg_sheet.png");
		
		// Go Back Button
		SDL_Rect idle5 = { 382, 508, 37, 36 };
		SDL_Rect hovered5 = { 343, 508, 37, 36 };
		SDL_Rect clicked5 = { 421, 508, 37, 36 };
		App->gui->CreateButton(&key_buttons, BUTTON, 7, 7, idle5, hovered5, clicked5, key_tex, GO_TO_MENU);
		
		// Normal button
		SDL_Rect idle = { 631, 12, 151, 38 };
		SDL_Rect hovered = { 963, 12, 151, 38 };
		SDL_Rect clicked = { 797, 12, 151, 38 };

		App->gui->CreateButton(&key_buttons, BUTTON, 60, 60, idle, hovered, clicked, key_tex, MOVE_UP_BUT);
		App->gui->CreateButton(&key_buttons, BUTTON, 60, 100, idle, hovered, clicked, key_tex, MOVE_DOWN_BUT);
		App->gui->CreateButton(&key_buttons, BUTTON, 60, 140, idle, hovered, clicked, key_tex, MOVE_RIGHT_BUT);
		App->gui->CreateButton(&key_buttons, BUTTON, 60, 180, idle, hovered, clicked, key_tex, MOVE_LEFT_BUT);
		App->gui->CreateButton(&key_buttons, BUTTON, 60, 220, idle, hovered, clicked, key_tex, TABULADOR_BUT);

		App->gui->CreateLabel(&key_labels, LABEL, 60 + 18, 60 + 1, App->gui->font1, "MOVE UP", App->gui->beige);
		App->gui->CreateLabel(&key_labels, LABEL, 60 + 14, 100 + 1, App->gui->font1, "MOVE DOWN", App->gui->beige);
		App->gui->CreateLabel(&key_labels, LABEL, 60 + 13, 140 + 1, App->gui->font1, "MOVE RIGHT", App->gui->beige);
		App->gui->CreateLabel(&key_labels, LABEL, 60 + 14, 180 + 1, App->gui->font1, "MOVE LEFT", App->gui->beige);
		App->gui->CreateLabel(&key_labels, LABEL, 60 + 11, 220 + 1, App->gui->font1, "SHOW STATS", App->gui->beige);

		App->gui->CreateButton(&key_buttons, BUTTON, 200, 60, idle, hovered, clicked, key_tex, NORMAL_ATTACK_BUT);
		App->gui->CreateButton(&key_buttons, BUTTON, 200, 100, idle, hovered, clicked, key_tex, ABILITY1_BUT);
		App->gui->CreateButton(&key_buttons, BUTTON, 200, 140, idle, hovered, clicked, key_tex, ABILITY2_BUT);
		App->gui->CreateButton(&key_buttons, BUTTON, 200, 180, idle, hovered, clicked, key_tex, BUY_ITEM_BUT);
		App->gui->CreateButton(&key_buttons, BUTTON, 200, 220, idle, hovered, clicked, key_tex, USE_POTION_BUT);

		App->gui->CreateLabel(&key_labels, LABEL, 200 + 19, 60 + 1, App->gui->font1, "ATTACK", App->gui->beige);
		App->gui->CreateLabel(&key_labels, LABEL, 200 + 17, 100 + 1, App->gui->font1, "ABILITY 1", App->gui->beige);
		App->gui->CreateLabel(&key_labels, LABEL, 200 + 17, 140 + 1, App->gui->font1, "ABILITY 2", App->gui->beige);
		App->gui->CreateLabel(&key_labels, LABEL, 200 + 16, 180 + 1, App->gui->font1, "BUY ITEM", App->gui->beige);
		App->gui->CreateLabel(&key_labels, LABEL, 200 + 14, 220 + 1, App->gui->font1, "USE POTION", App->gui->beige);

		
	}
	return true;
}

bool j1SceneKeyConfig::Update(float dt) {

	App->gui->UpdateButtonsState(&key_buttons, App->gui->buttonsScale);

	for (std::list<j1Button*>::iterator item = key_buttons.begin(); item != key_buttons.end(); ++item) {
		switch ((*item)->state)
		{
		case IDLE:
			(*item)->situation = (*item)->idle;
			break;

		case HOVERED:
			if (startup_time.Read() > 2000)
				(*item)->situation = (*item)->hovered;
			break;

		case RELEASED:
			if (startup_time.Read() > 2000) {
				(*item)->situation = (*item)->idle;
				if ((*item)->bfunction == GO_TO_MENU)
					App->transitions->Wiping(KEY_CHANGES, SCENE_SETTINGS);
			}
			break;

		case CLICKED:
			if (startup_time.Read() > 2000)
				(*item)->situation = (*item)->clicked;
			break;
		}

	}

	App->map->Draw();

	for (std::list<j1Button*>::iterator item = key_buttons.begin(); item != key_buttons.end(); ++item) {
		(*item)->Draw(App->gui->buttonsScale);
	}
	for (std::list<j1Label*>::iterator item = key_labels.begin(); item != key_labels.end(); ++item) {
		(*item)->Draw();
	}

	return true;
}

bool j1SceneKeyConfig::PostUpdate() {
	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		continueGame = false;

	return continueGame;
}

bool j1SceneKeyConfig::CleanUp() {

	LOG("Freeing Key Scene Textures");

	App->tex->UnLoad(key_tex);
	App->map->CleanUp();
	App->tex->CleanUp();

	for (std::list<j1Button*>::iterator item = key_buttons.begin(); item != key_buttons.end(); ++item) {
		(*item)->CleanUp();
		key_buttons.remove(*item);
	}

	for (std::list<j1Label*>::iterator item = key_labels.begin(); item != key_labels.end(); ++item) {
		(*item)->CleanUp();
		key_labels.remove(*item);
	}

	for (std::list<j1Box*>::iterator item = key_boxes.begin(); item != key_boxes.end(); ++item) {
		(*item)->CleanUp();
		key_boxes.remove(*item);
	}

	return true;
}

void j1SceneKeyConfig::ChangeKey(KEY_ACTIONS key_to_change)
{

}