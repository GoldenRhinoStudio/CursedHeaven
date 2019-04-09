#include "p2Defs.h"
#include "p2Log.h"

#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Collisions.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1DragoonKnight.h"
#include "j1Judge.h"
#include "j1SceneMenu.h"
#include "j1Scene1.h"
#include "j1FadeToBlack.h"
#include "j1Pathfinding.h"
#include "j1Gui.h"
#include "j1SceneMenu.h"
#include "j1Fonts.h"
#include "j1Label.h"
#include "j1Button.h"
#include "j1Box.h"
#include "j1ChooseCharacter.h"
#include "j1DialogSystem.h"

#include "Brofiler/Brofiler.h"

j1ChooseCharacter::j1ChooseCharacter() : j1Module() {
	name.assign("choose_character");
}

j1ChooseCharacter::~j1ChooseCharacter()
{}

bool j1ChooseCharacter::Awake(pugi::xml_node& config) {
	LOG("Loading Choose Character Scene");
	bool ret = true;

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

		// Loading textures
		graphics = App->tex->Load("textures/choose_character/bg.png");
		button_tex = App->tex->Load("textures/choose_character/CHARACTERS_LOCKED.png");

		SDL_Rect BG_idle = { 0,0,234,494 };
		SDL_Rect BG_hover = { 468,0,234,494 };
		SDL_Rect BG_click = { 936,0,234,494 };

		App->gui->CreateButton(&chooseCharacterButtons, BUTTON, 12, 55, BG_idle, BG_hover, BG_hover, button_tex, BLACKMAGE_BUT);

		SDL_Rect DK_idle = { 0,494,234,494 };
		SDL_Rect DK_hover = { 468,494,234,494 };

		App->gui->CreateButton(&chooseCharacterButtons, BUTTON, 135, 55, DK_idle, DK_hover, DK_hover, button_tex, DRAGOONKNIGHT_BUT);

		SDL_Rect RG_idle = { 234,0,234,494 };
		SDL_Rect RG_hover = { 702,0,234,494 };

		App->gui->CreateButton(&chooseCharacterButtons, BUTTON, 258, 55, RG_idle, RG_idle, RG_idle, button_tex, ROGUE_BUT);

		SDL_Rect TK_idle = { 234,494,234,494 };
		SDL_Rect TK_hover = { 702,494,234,494 };

		App->gui->CreateButton(&chooseCharacterButtons, BUTTON, 381, 55, TK_idle, TK_idle, TK_idle, button_tex, TANK_BUT);

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

	App->gui->UpdateButtonsState(&chooseCharacterButtons);

	for (std::list<j1Button*>::iterator item = chooseCharacterButtons.begin(); item != chooseCharacterButtons.end(); ++item) {
		if ((*item)->visible) {
			switch ((*item)->state) {
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
					if ((*item)->bfunction == BLACKMAGE_BUT) {
						startGame = true;
						App->fade->FadeToBlack();
						App->entity->player_type = MAGE;
						LOG("Black Mage activated");
					}
					else if ((*item)->bfunction == ROGUE_BUT) {
						// startGame = true;
						// App->fade->FadeToBlack();
						// App->entity->player_type = ROGUE;
						// LOG("Rogue activated");
					}
					else if ((*item)->bfunction == DRAGOONKNIGHT_BUT) {
						startGame = true;
						App->fade->FadeToBlack();
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

	background = { 0,0,1024,768 };

	App->render->Blit(graphics, 0, 0, &background);

	if (App->fade->IsFading() == 0) {
		if (startGame) {
			ChangeScene();
			LOG("Scene1");
			App->menu->player_created = true;
		}
	}

	for (std::list<j1Button*>::iterator item = chooseCharacterButtons.begin(); item != chooseCharacterButtons.end(); ++item) {
		(*item)->Draw(App->gui->buttonsScale);
	}

	return true;
}

bool j1ChooseCharacter::CleanUp() {
	LOG("Freeing all textures");

	App->tex->UnLoad(button_tex);
	App->tex->UnLoad(graphics);

	App->tex->CleanUp();

	for (std::list<j1Button*>::iterator item = chooseCharacterButtons.begin(); item != chooseCharacterButtons.end(); ++item) {
		(*item)->CleanUp();
		chooseCharacterButtons.remove(*item);
	}

	return true;
}

void j1ChooseCharacter::ChangeScene() {
	
	this->active = false;
	startGame = false;

	CleanUp();

	App->scene1->active = true;
	App->scene1->Start();
	App->dialog->active = true;
	App->dialog->Start();
	
	App->entity->active = true;
	App->entity->CreatePlayer();
	App->entity->CreateNPC();
	App->entity->Start();
}