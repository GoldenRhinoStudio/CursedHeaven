#include "p2Defs.h"
#include "p2Log.h"

#include "j1App.h"
#include "j1Map.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Collisions.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1DragoonKnight.h"
#include "j1Judge.h"
#include "j1SceneMenu.h"
#include "j1Scene1.h"
#include "j1FadeToBlack.h"
#include "j1Pathfinding.h"
#include "j1Gui.h"
#include "j1Shop.h"
#include "j1SceneMenu.h"
#include "j1Fonts.h"
#include "j1Label.h"
#include "j1Button.h"
#include "j1Box.h"
#include "j1ChooseCharacter.h"
#include "j1DialogSystem.h"
#include "j1Particles.h"
#include "j1TransitionManager.h"
#include "j1Video.h"
#include "Brofiler/Brofiler.h"

j1ChooseCharacter::j1ChooseCharacter() : j1Module() {
	name.assign("choose_character");
}

j1ChooseCharacter::~j1ChooseCharacter()
{}

bool j1ChooseCharacter::Awake(pugi::xml_node& config) {
	LOG("Loading Choose Character Scene");
	bool ret = true;
	if (App->video->active)
		active = false;
	if (App->menu->active)
		active = false;
	else if (!App->menu->active && !App->scene1->active)
		active = true;

	if (App->choose_character->active == false)
	{
		LOG("Unable to load the choose character scene.");
		ret = false;
	}

	return ret;
}

bool j1ChooseCharacter::Start() {

	if (active) {
		// The map is loaded
		App->map->draw_with_quadtrees = false;
		App->map->Load("menu.tmx");

		// Loading textures
		button_tex = App->tex->Load("gui/CHARACTERS_LOCKED.png");
		gui_tex2 = App->tex->Load("gui/uipack_rpg_sheet.png");
		info_tex = App->tex->Load("gui/uipack_rpg_sheet_2.png");

		SDL_Rect invisible = { 0,0, SCREEN_WIDTH, SCREEN_HEIGHT };
		App->gui->CreateButton(&chooseCharacterButtons, BUTTON, 2, 40, invisible, invisible, invisible, NULL, NONE_BUT);
		
		SDL_Rect name_box = { 9, 12, 1053, 352 };
		info_window = App->gui->CreateBox(&chooseCharacterBoxes, BOX, 22, 150, name_box, info_tex);

		SDL_Rect BG_idle = { 0,0,163,310 };
		SDL_Rect BG_hover = { 327,0,164,310 };
		App->gui->CreateButton(&chooseCharacterButtons, BUTTON, 2 + 12, 40 - 20, BG_idle, BG_hover, BG_hover, button_tex, BLACKMAGE_BUT);

		SDL_Rect DK_idle = { 0,346,163,310 };
		SDL_Rect DK_hover = { 327,346,164,310 };
		App->gui->CreateButton(&chooseCharacterButtons, BUTTON, 87 + 12, 40 - 20, DK_idle, DK_hover, DK_hover, button_tex, DRAGOONKNIGHT_BUT);

		SDL_Rect RG_idle = { 164,0,164,310 };
		SDL_Rect RG_hover = { 489,0,164,310 };
		App->gui->CreateButton(&chooseCharacterButtons, BUTTON, 172 + 12, 40 - 20, RG_idle, RG_idle, RG_idle, button_tex, ROGUE_BUT);

		SDL_Rect TK_idle = { 164,346,163,310 };
		SDL_Rect TK_hover = { 489,346,163,310 };
		App->gui->CreateButton(&chooseCharacterButtons, BUTTON, 257 + 12, 40 - 20, TK_idle, TK_idle, TK_idle, button_tex, TANK_BUT);

		App->menu->player_created = false;

		startup_time.Start();
	}

	return true;
}

bool j1ChooseCharacter::PreUpdate()
{
	BROFILER_CATEGORY("ChooseCharacterPreUpdate", Profiler::Color::Orange)

	return true;
}

bool j1ChooseCharacter::Update(float dt) {

	BROFILER_CATEGORY("ChooseCharacterUpdate", Profiler::Color::LightSeaGreen)

	App->gui->UpdateButtonsState(&chooseCharacterButtons, 0.35f);

	for (std::list<j1Button*>::iterator item = chooseCharacterButtons.begin(); item != chooseCharacterButtons.end(); ++item) {
		if ((*item)->visible) {
			switch ((*item)->state) {
			case IDLE:
				(*item)->situation = (*item)->idle;
				break;

			case HOVERED:
				if (startup_time.Read() > 2000) {
					(*item)->situation = (*item)->hovered;
					if ((*item)->bfunction == BLACKMAGE_BUT) {
						App->gui->CreateLabel(&chooseCharacterLabels, LABEL, 130, 155, App->gui->font2, "Black Mage", App->gui->brown);
						App->gui->CreateLabel(&chooseCharacterLabels, LABEL, 80, 195, App->gui->font1, "A fast wizard that uses his powerful ranged", App->gui->beige);
						App->gui->CreateLabel(&chooseCharacterLabels, LABEL, 96, 210, App->gui->font1, "attacks and deals high area damage", App->gui->beige);
					}
					else if ((*item)->bfunction == DRAGOONKNIGHT_BUT) {
						App->gui->CreateLabel(&chooseCharacterLabels, LABEL, 110, 155, App->gui->font2, "Dragoon Knight", App->gui->brown);
						App->gui->CreateLabel(&chooseCharacterLabels, LABEL, 70, 195, App->gui->font1, "An agile warrior that moves fast acroos the map", App->gui->beige);
						App->gui->CreateLabel(&chooseCharacterLabels, LABEL, 89, 210, App->gui->font1, "and deals great damage with his sword", App->gui->beige);
					}
					else if ((*item)->bfunction == ROGUE_BUT) {
						App->gui->CreateLabel(&chooseCharacterLabels, LABEL, 140, 180, App->gui->font2, "LOCKED", App->gui->brown);
					}
					else if ((*item)->bfunction == TANK_BUT) {
						App->gui->CreateLabel(&chooseCharacterLabels, LABEL, 140, 180, App->gui->font2, "LOCKED", App->gui->brown);
					}
					else if ((*item)->bfunction == NONE_BUT) {
						for (std::list<j1Label*>::iterator item = chooseCharacterLabels.begin(); item != chooseCharacterLabels.end(); ++item) {
							(*item)->CleanUp();
							chooseCharacterLabels.remove(*item);
						}
					}
				}
				break;

			case RELEASED:
				if (startup_time.Read() > 2000) {
					(*item)->situation = (*item)->idle;
					if ((*item)->bfunction == BLACKMAGE_BUT && App->entity->player_type == NO_PLAYER) {
						App->transitions->SquaresAppearing(CHOOSE, SCENE1, 3);
						App->entity->player_type = MAGE;
						LOG("Black Mage activated");
					}
					else if ((*item)->bfunction == ROGUE_BUT) {
						// startGame = true;
						// App->fade->FadeToBlack();
						// App->entity->player_type = ROGUE;
						// LOG("Rogue activated");
					}
					else if ((*item)->bfunction == DRAGOONKNIGHT_BUT && App->entity->player_type == NO_PLAYER) {
						App->transitions->SquaresAppearing(CHOOSE, SCENE1, 3);
						App->entity->player_type = KNIGHT;
						LOG("Dragoon Knight activated");
					}
					else if ((*item)->bfunction == TANK_BUT) {
						// startGame = true;
						// App->fade->FadeToBlack();
						// App->entity->player_type = TANK;
						// LOG("Tank activated");
					}
				}
				break;

			case CLICKED:
				if (startup_time.Read() > 2000)
				(*item)->situation = (*item)->clicked;
				break;
			}
		}
	}

	App->map->Draw();

	for (std::list<j1Button*>::iterator item = chooseCharacterButtons.begin(); item != chooseCharacterButtons.end(); ++item) {
		(*item)->Draw(0.35f);
	}

	for (std::list<j1Label*>::iterator item = chooseCharacterLabels.begin(); item != chooseCharacterLabels.end(); ++item) {
		(*item)->Draw();
	}

	for (std::list<j1Box*>::iterator item = chooseCharacterBoxes.begin(); item != chooseCharacterBoxes.end(); ++item) {
		(*item)->Draw(App->gui->settingsWindowScale);
	}

	// Blitting labels of the menu
	for (std::list<j1Label*>::iterator item = chooseCharacterLabels.begin(); item != chooseCharacterLabels.end(); ++item)
	{
		if ((*item)->parent != nullptr) continue;
		if ((*item)->visible) (*item)->Draw();
	}

	return true;
}

bool j1ChooseCharacter::CleanUp() {
	LOG("Freeing all textures");

	App->tex->UnLoad(button_tex);
	App->map->CleanUp();
	App->tex->CleanUp();

	for (std::list<j1Button*>::iterator item = chooseCharacterButtons.begin(); item != chooseCharacterButtons.end(); ++item) {
		(*item)->CleanUp();
		chooseCharacterButtons.remove(*item);
	}

	for (std::list<j1Label*>::iterator item = chooseCharacterLabels.begin(); item != chooseCharacterLabels.end(); ++item) {
		(*item)->CleanUp();
		chooseCharacterLabels.remove(*item);
	}

	for (std::list<j1Box*>::iterator item = chooseCharacterBoxes.begin(); item != chooseCharacterBoxes.end(); ++item) {
		(*item)->CleanUp();
		chooseCharacterBoxes.remove(*item);
	}

	return true;
}

