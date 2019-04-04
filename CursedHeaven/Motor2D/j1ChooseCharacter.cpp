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

#include "Brofiler/Brofiler.h"

j1ChooseCharacter::j1ChooseCharacter() : j1Module() {
	name.assign("choose_character");
}

j1ChooseCharacter::~j1ChooseCharacter()
{}

bool j1ChooseCharacter::Awake(pugi::xml_node& config) {
	LOG("Loading Choose Character Scene");
	bool ret = true;

	if (App->menu->active = true)
		active = false;

	if (App->choose_character->active == false)
	{
		LOG("Unable to load the choose character scene.");
		ret = false;
	}

	return ret;
}

bool j1ChooseCharacter::Start() {

	if (active) {

		graphics = App->tex->Load("textures/choose_character/bg.png");
		button_tex = App->tex->Load("textures/choose_character/characters.png");

		background = { 0,0,1500,1007 };

		App->render->Blit(graphics, 0, 0, &background);

		SDL_Rect BG_idle = { 0,0,570,1200 };
		SDL_Rect BG_hover = { 1140,0,600,1263 };

		App->gui->CreateButton(&chooseCharacterButtons, BUTTON, 0, 0, BG_idle, BG_hover, BG_hover, button_tex, BLACKMAGE_BUT);

		SDL_Rect DK_idle = { 0,1200,570,1200 };
		SDL_Rect DK_hover = { 1140,1263,600,1263 };

		App->gui->CreateButton(&chooseCharacterButtons, BUTTON, 600, 0, DK_idle, DK_hover, DK_hover, button_tex, DRAGOONKNIGHT_BUT);

		SDL_Rect RG_idle = { 570,0,570,1200 };
		SDL_Rect RG_hover = { 1740,0,600,1263 };

		App->gui->CreateButton(&chooseCharacterButtons, BUTTON, 1200, 0, RG_idle, RG_hover, RG_hover, button_tex, ROGUE_BUT);

		SDL_Rect TK_idle = { 570,1200,570,1200 };
		SDL_Rect TK_hover = { 1740,1263,600,1263 };

		App->gui->CreateButton(&chooseCharacterButtons, BUTTON, 1800, 0, TK_idle, TK_hover, TK_hover, button_tex, TANK_BUT);
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
				(*item)->situation = (*item)->hovered;

			case RELEASED:
				(*item)->situation = (*item)->idle;
				if ((*item)->bfunction == BLACKMAGE_BUT) {
					App->menu->startGame = true;
					App->fade->FadeToBlack();
				}
				else if ((*item)->bfunction == ROGUE_BUT) {
					App->menu->startGame = true;
					App->fade->FadeToBlack();
				}
				else if ((*item)->bfunction == DRAGOONKNIGHT_BUT) {
					App->menu->startGame = true;
					App->fade->FadeToBlack();
				}
				else if ((*item)->bfunction == TANK_BUT) {
					App->menu->startGame = true;
					App->fade->FadeToBlack();
				}

			case CLICKED:
				(*item)->situation = (*item)->clicked;
			}
		}
	}

	if (App->fade->IsFading() == 0) {
		if (App->menu->startGame) {
			ChangeScene();
			LOG("Scene1");
			App->menu->player_created = true;
		}
	}

	for (std::list<j1Button*>::iterator item = chooseCharacterButtons.begin(); item != chooseCharacterButtons.end(); ++item) {
		if ((*item)->parent == nullptr) continue;

		if ((*item)->parent->visible == false)
			(*item)->visible = false;
		else
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

	CleanUp();

	App->scene1->active = true;
	App->scene1->Start();
	
	App->entity->active = true;
	App->entity->CreatePlayer();
	App->entity->CreateNPC();
	App->entity->Start();
}