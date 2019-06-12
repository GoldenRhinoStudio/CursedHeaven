#define _CRT_SECURE_NO_WARNINGS

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
#include "j1Video.h"
#include "Brofiler/Brofiler.h"

j1SceneKeyConfig::j1SceneKeyConfig()
{
	name.assign("key_config");
}

j1SceneKeyConfig::~j1SceneKeyConfig()
{}

bool j1SceneKeyConfig::Awake(pugi::xml_node &) {
	LOG("Loading Key Config");

	bool ret = true;
	if (App->video->active) active = false;
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

		up = App->gui->CreateLabel(&key_labels, LABEL, 60 + 2, 60 + 1, App->gui->font1, "MOVE UP   W", App->gui->beige);
		down = App->gui->CreateLabel(&key_labels, LABEL, 60 + 2, 100 + 1, App->gui->font1, "MOVE DOWN   S", App->gui->beige);
		right = App->gui->CreateLabel(&key_labels, LABEL, 60 + 2, 140 + 1, App->gui->font1, "MOVE RIGHT   D", App->gui->beige);
		left = App->gui->CreateLabel(&key_labels, LABEL, 60 + 2, 180 + 1, App->gui->font1, "MOVE LEFT   A", App->gui->beige);
		stats = App->gui->CreateLabel(&key_labels, LABEL, 60 + 2, 220 + 1, App->gui->font1, "STATS   TAB", App->gui->beige);

		App->gui->CreateButton(&key_buttons, BUTTON, 200, 60, idle, hovered, clicked, key_tex, ABILITY1_BUT);
		App->gui->CreateButton(&key_buttons, BUTTON, 200, 100, idle, hovered, clicked, key_tex, ABILITY2_BUT);
		App->gui->CreateButton(&key_buttons, BUTTON, 200, 140, idle, hovered, clicked, key_tex, BUY_ITEM_BUT);
		App->gui->CreateButton(&key_buttons, BUTTON, 200, 180, idle, hovered, clicked, key_tex, USE_POTION_BUT);

		ability1 = App->gui->CreateLabel(&key_labels, LABEL, 200 + 2, 60 + 1, App->gui->font1, "ABILITY 1   Q", App->gui->beige);
		ability2 = App->gui->CreateLabel(&key_labels, LABEL, 200 + 2, 100 + 1, App->gui->font1, "ABILITY 2   E", App->gui->beige);
		item = App->gui->CreateLabel(&key_labels, LABEL, 200 + 2, 140 + 1, App->gui->font1, "BUY ITEM   ENTER", App->gui->beige);
		potion = App->gui->CreateLabel(&key_labels, LABEL, 200 + 2, 180 + 1, App->gui->font1, "USE POTION   R", App->gui->beige);

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
				if (!waiting) {
					(*item)->situation = (*item)->idle;

					if ((*item)->bfunction == GO_TO_MENU)
						App->transitions->Wiping(KEY_CHANGES, SCENE_SETTINGS);

					else if ((*item)->bfunction != GO_TO_MENU) {
						waiting = true;
						App->input->last_key_pressed = -1;
						App->input->button_pressed = false;
					}

					if (((*item)->bfunction) == MOVE_UP_BUT) {
						last_changed = 1;
					}
					else if (((*item)->bfunction) == MOVE_DOWN_BUT) {
						last_changed = 2;
					}
					else if (((*item)->bfunction) == MOVE_LEFT_BUT) {
						last_changed = 3;
					}
					else if (((*item)->bfunction) == MOVE_RIGHT_BUT) {
						last_changed = 4;
					}
					else if (((*item)->bfunction) == TABULADOR_BUT) {
						last_changed = 6;
					}
					else if (((*item)->bfunction) == BUY_ITEM_BUT) {
						last_changed = 7;
					}
					else if (((*item)->bfunction) == USE_POTION_BUT) {
						last_changed = 8;
					}
					else if (((*item)->bfunction) == ABILITY1_BUT) {
						last_changed = 9;
					}
					else if (((*item)->bfunction) == ABILITY2_BUT) {
						last_changed = 10;
					}

				}
			}
			break;

		case CLICKED:
			if (startup_time.Read() > 2000)
				(*item)->situation = (*item)->clicked;
			break;
		}
		
		if (waiting && App->input->button_pressed) {
			
			if (last_changed == 1) {
				MOVE_UP = ChangeKey(MOVE_UP);
			}
			else if (last_changed == 2) {
				MOVE_DOWN = ChangeKey(MOVE_DOWN);
			}
			else if (last_changed == 3) {
				MOVE_LEFT = ChangeKey(MOVE_LEFT);
			}
			else if (last_changed == 4) {
				MOVE_RIGHT = ChangeKey(MOVE_RIGHT);
			}
			if (last_changed == 6) {
				TABULADOR = ChangeKey(TABULADOR);
			}
			else if (last_changed == 7) {
				BUY_ITEM = ChangeKey(BUY_ITEM);
			}
			else if (last_changed == 8) {
				USE_POTION = ChangeKey(USE_POTION);
			}
			else if (last_changed == 9) {
				ABILITY1 = ChangeKey(ABILITY1);
			}
			else if (last_changed == 10) {
				ABILITY2 = ChangeKey(ABILITY2);
			}

			App->audio->PlayFx(App->audio->change_key_sound);
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

int j1SceneKeyConfig::ChangeKey(int key_to_change)
{
	j1Label* key_label = nullptr;
	char* button = nullptr;

	switch (last_changed)
	{
	case 1:
		key_label = up;
		button = "MOVE UP";
		break;
	case 2:
		key_label = down;
		button = "MOVE DOWN";
		break;
	case 3:
		key_label = left;
		button = "MOVE LEFT";
		break;
	case 4: 
		key_label = right;
		button = "MOVE RIGHT";
		break;
	case 6:
		key_label = stats;
		button = "SHOW STATS";
		break;
	case 7:
		key_label = item;
		button = "BUY ITEM";
		break;
	case 8: 
		key_label = potion;
		button = "USE POTION";
		break;
	case 9:
		key_label = ability1;
		button = "ABILITY 1";
		break;
	case 10:
		key_label = ability2;
		button = "ABILITY 2";
		break;
	}

	int new_key;

	if (App->input->last_key_pressed != -1)
		new_key = App->input->last_key_pressed;

	else
		new_key = key_to_change;


	char* letter = nullptr;
	switch (new_key)
	{
	case 4:
		letter = "   A";
		strcpy(text, button);
		strcat(text, letter);
		key_label->text = text;
		break;
	case 5:
		letter = "   B";
		strcpy(text2, button);
		strcat(text2, letter);
		key_label->text = text2;
		break;
	case 6:
		letter = "   C";
		strcpy(text3, button);
		strcat(text3, letter);
		key_label->text = text3;
		break;
	case 7:
		letter = "   D";
		strcpy(text4, button);
		strcat(text4, letter);
		key_label->text = text4;
		break;
	case 8:
		letter = "   E";
		strcpy(text5, button);
		strcat(text5, letter);
		key_label->text = text5;
		break;
	case 9:
		letter = "   F";
		strcpy(text6, button);
		strcat(text6, letter);
		key_label->text = text6;
		break;
	case 10:
		letter = "   G";
		strcpy(text7, button);
		strcat(text7, letter);
		key_label->text = text7;
		break;
	case 11:
		letter = "   H";
		strcpy(text8, button);
		strcat(text8, letter);
		key_label->text = text8;
		break;
	case 12:
		letter = "   I";
		strcpy(text9, button);
		strcat(text9, letter);
		key_label->text = text9;
		break;
	case 13:
		letter = "   J";
		strcpy(text10, button);
		strcat(text10, letter);
		key_label->text = text10;
		break;
	case 14:
		letter = "   K";
		strcpy(text11, button);
		strcat(text11, letter);
		key_label->text = text11;
		break;
	case 15:
		letter = "   L";
		strcpy(text12, button);
		strcat(text12, letter);
		key_label->text = text12;
		break;
	case 16:
		letter = "   M";
		strcpy(text13, button);
		strcat(text13, letter);
		key_label->text = text13;
		break;
	case 17:
		letter = "   N";
		strcpy(text14, button);
		strcat(text14, letter);
		key_label->text = text14;
		break;
	case 18:
		letter = "   O";
		strcpy(text15, button);
		strcat(text15, letter);
		key_label->text = text15;
		break;
	case 19:
		letter = "   P";
		strcpy(text16, button);
		strcat(text16, letter);
		key_label->text = text16;
		break;
	case 20:
		letter = "   Q";
		strcpy(text17, button);
		strcat(text17, letter);
		key_label->text = text17;
		break;
	case 21:
		letter = "   R";
		strcpy(text18, button);
		strcat(text18, letter);
		key_label->text = text18;
		break;
	case 22:
		letter = "   S";
		strcpy(text19, button);
		strcat(text19, letter);
		key_label->text = text19;
		break;
	case 23:
		letter = "   T";
		strcpy(text20, button);
		strcat(text20, letter);
		key_label->text = text20;
		break;
	case 24:
		letter = "   U";
		strcpy(text21, button);
		strcat(text21, letter);
		key_label->text = text21;
		break;
	case 25:
		letter = "   V";
		strcpy(text22, button);
		strcat(text22, letter);
		key_label->text = text22;
		break;
	case 26:
		letter = "   W";
		strcpy(text23, button);
		strcat(text23, letter);
		key_label->text = text23;
		break;
	case 27:
		letter = "   X";
		strcpy(text24, button);
		strcat(text24, letter);
		key_label->text = text24;
		break;
	case 28:
		letter = "   Y";
		strcpy(text25, button);
		strcat(text25, letter);
		key_label->text = text25;
		break;
	case 29:
		letter = "   Z";
		strcpy(text26, button);
		strcat(text26, letter);
		key_label->text = text26;
		break;
	case 51:
		letter = "   Ñ";
		strcpy(text27, button);
		strcat(text27, letter);
		key_label->text = text27;
		break;
	case 49:
		letter = "   Ç";
		strcpy(text28, button);
		strcat(text28, letter);
		key_label->text = text28;
		break;
	case 43:
		letter = "   TAB";
		strcpy(text29, button);
		strcat(text29, letter);
		key_label->text = text29;
		break;
	case 40:
		letter = "   ENTER";
		strcpy(text30, button);
		strcat(text30, letter);
		key_label->text = text30;
		break;
	case 44:
		letter = "   SPACE";
		strcpy(text31, button);
		strcat(text31, letter);
		key_label->text = text31;
		break;
	case 225:
		letter = "   SHIFT";
		strcpy(text32, button);
		strcat(text32, letter);
		key_label->text = text32;
		break;
	
	case 224:
		letter = "   CTRL";
		strcpy(text33, button);
		strcat(text33, letter);
		key_label->text = text33;
		break;
	}


	waiting = false;



	return new_key;
}