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
#include "j1SceneMenu.h"
#include "j1Fonts.h"
#include "j1Label.h"
#include "j1Button.h"
#include "j1Box.h"
#include "j1ChooseCharacter.h"
#include "j1DialogSystem.h"
#include "j1Particles.h"

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

		// The map is loaded
		App->map->Load("menu.tmx");

		// Loading textures
		button_tex = App->tex->Load("textures/choose_character/CHARACTERS_LOCKED.png");

		// Loading fonts
		font = App->font->Load("fonts/Pixeled.ttf", 8);

		SDL_Rect BG_idle = { 0,0,163,346 };
		SDL_Rect BG_hover = { 327,0,164,346 };

		App->gui->CreateButton(&chooseCharacterButtons, BUTTON, 2, 40, BG_idle, BG_hover, BG_hover, button_tex, BLACKMAGE_BUT);

		SDL_Rect DK_idle = { 0,346,163,346 };
		SDL_Rect DK_hover = { 327,346,164,346 };

		App->gui->CreateButton(&chooseCharacterButtons, BUTTON, 87, 40, DK_idle, DK_hover, DK_hover, button_tex, DRAGOONKNIGHT_BUT);

		SDL_Rect RG_idle = { 164,0,164,346 };
		SDL_Rect RG_hover = { 489,0,164,346 };

		App->gui->CreateButton(&chooseCharacterButtons, BUTTON, 172, 40, RG_idle, RG_idle, RG_idle, button_tex, ROGUE_BUT);

		SDL_Rect TK_idle = { 164,346,163,346 };
		SDL_Rect TK_hover = { 489,346,163,346 };

		App->gui->CreateButton(&chooseCharacterButtons, BUTTON, 257, 40, TK_idle, TK_idle, TK_idle, button_tex, TANK_BUT);

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

	App->gui->UpdateButtonsState(&chooseCharacterButtons, App->gui->buttonsScale);

	for (std::list<j1Button*>::iterator item = chooseCharacterButtons.begin(); item != chooseCharacterButtons.end(); ++item) {
		if ((*item)->visible) {
			switch ((*item)->state) {
			case IDLE:
				(*item)->situation = (*item)->idle;
				break;

			case HOVERED:
				if (startup_time.Read() > 2000)
				(*item)->situation = (*item)->hovered;
				if ((*item)->bfunction == BLACKMAGE_BUT) 
					App->gui->CreateLabel(&chooseCharacterLabels, LABEL, 156, 220, font, "BlackMague", App->gui->beige);
				else if ((*item)->bfunction == DRAGOONKNIGHT_BUT)
					App->gui->CreateLabel(&chooseCharacterLabels, LABEL, 156, 220, font, "dk", App->gui->beige);
				else {
					for (std::list<j1Label*>::iterator item = chooseCharacterLabels.begin(); item != chooseCharacterLabels.end(); ++item) {
						(*item)->CleanUp();
						chooseCharacterLabels.remove(*item);
					}
				}
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

	if (App->fade->IsFading() == 0) {
		if (startGame) {
			ChangeScene();
			LOG("Scene1");
			App->menu->player_created = true;
		}
	}

	App->map->Draw();

	for (std::list<j1Button*>::iterator item = chooseCharacterButtons.begin(); item != chooseCharacterButtons.end(); ++item) {
		(*item)->Draw(App->gui->buttonsScale);
	}

	for (std::list<j1Label*>::iterator item = chooseCharacterLabels.begin(); item != chooseCharacterLabels.end(); ++item) {
		(*item)->Draw();
	}

	return true;
}

bool j1ChooseCharacter::CleanUp() {
	LOG("Freeing all textures");

	App->tex->UnLoad(button_tex);

	App->tex->CleanUp();

	for (std::list<j1Button*>::iterator item = chooseCharacterButtons.begin(); item != chooseCharacterButtons.end(); ++item) {
		(*item)->CleanUp();
		chooseCharacterButtons.remove(*item);
	}

	for (std::list<j1Label*>::iterator item = chooseCharacterLabels.begin(); item != chooseCharacterLabels.end(); ++item) {
		(*item)->CleanUp();
		chooseCharacterLabels.remove(*item);
	}

	return true;
}

void j1ChooseCharacter::ChangeScene() {
	
	this->active = false;
	startGame = false;

	CleanUp();

	App->scene1->active = true;
	App->scene1->Start();
	App->particles->Start();
	App->dialog->active = true;
	App->dialog->Start();
	
	App->entity->active = true;
	App->entity->CreatePlayer();
	//App->entity->CreateNPC();
	App->entity->Start();
}

void j1ChooseCharacter::CharacterInformation()
{
	
	
}
