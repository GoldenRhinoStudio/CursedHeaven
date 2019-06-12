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
#include "j1BlackMage.h"
#include "j1Tank.h"
#include "j1Rogue.h"
#include "j1Judge.h"
#include "j1SceneMenu.h"
#include "j1Scene1.h"
#include "j1Scene2.h"
#include "j1FadeToBlack.h"
#include "j1Pathfinding.h"
#include "j1Gui.h"
#include "j1SceneMenu.h"
#include "j1Label.h"
#include "j1Button.h"
#include "j1Box.h"
#include "j1ChooseCharacter.h"
#include "j1DialogSystem.h"
#include "j1Particles.h"
#include "j1SceneLose.h"
#include "j1SceneVictory.h"
#include "j1Shop.h"
#include "j1Minimap.h"
#include "j1TransitionManager.h"
#include "j1Video.h"

#include "Brofiler/Brofiler.h"

j1Scene2::j1Scene2() : j1Module() {
	name.assign("scene2");
}

j1Scene2::~j1Scene2()
{}

bool j1Scene2::Awake(pugi::xml_node& config) {
	LOG("Loading Scene 2");

	bool ret = true;

	if (App->video->active)
		active = false;

	if (App->menu->active)
		active = false;

	if (App->scene1->active)
		active = false;

	if (!active)
		LOG("Scene2 not active.");

	return ret;
}

bool j1Scene2::Start() {

	if (active) {
		App->map->draw_with_quadtrees = true;

		if (App->map->Load("skycrown.tmx")) {
			int w, h;
			uchar* data = NULL;
			if (App->map->CreateWalkabilityMap(w, h, &data))
			{
				App->path->SetMap(w, h, data);
			}

			RELEASE_ARRAY(data);
		}

		// The audio is played	
		
		App->audio->PlayMusic("audio/music/song027.ogg", 1.0f);

		// Textures are loaded
		debug_tex = App->tex->Load("maps/path2.png");
		gui_tex = App->tex->Load("gui/uipack_rpg_sheet.png");
		lvl2_tex = App->tex->Load("maps/minimap_lvl2.png");
		bg = App->tex->Load("maps/Background.png");

		// Creating UI
		SDL_Rect section = { 9, 460, 315, 402 };
		settings_window = App->gui->CreateBox(&scene2Boxes, BOX, App->gui->settingsPosition.x, App->gui->settingsPosition.y, section, gui_tex);
		settings_window->visible = false;

		SDL_Rect idle = { 631, 12, 151, 38 };
		SDL_Rect hovered = { 963, 12, 151, 38 };
		SDL_Rect clicked = { 797, 14, 151, 37 };

		SDL_Rect slider_r = { 860,334,180,5 };
		App->gui->CreateButton(&scene2Buttons, BUTTON, 20, 65, slider_r, slider_r, slider_r, gui_tex, NO_FUNCTION, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateButton(&scene2Buttons, BUTTON, 20, 100, slider_r, slider_r, slider_r,  gui_tex, NO_FUNCTION, (j1UserInterfaceElement*)settings_window);

		App->gui->CreateBox(&scene2Boxes, BOX, 50, 55, { 388, 455, 28, 42 }, gui_tex, (j1UserInterfaceElement*)settings_window, 20, 92);
		App->gui->CreateBox(&scene2Boxes, BOX, 50, 90, { 388, 455, 28, 42 }, gui_tex, (j1UserInterfaceElement*)settings_window, 20, 92);

		App->gui->CreateButton(&scene2Buttons, BUTTON, 30, 20, idle, hovered, clicked, gui_tex, CLOSE_SETTINGS, (j1UserInterfaceElement*)settings_window);

		App->gui->CreateButton(&scene2Buttons, BUTTON, 30, 120, idle, hovered, clicked, gui_tex, GO_TO_MENU, (j1UserInterfaceElement*)settings_window);

		App->gui->CreateLabel(&scene2Labels, LABEL, 25, 40, App->gui->font2, "SOUND", App->gui->brown, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateLabel(&scene2Labels, LABEL, 25, 75, App->gui->font2, "MUSIC", App->gui->brown, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateLabel(&scene2Labels, LABEL, 48, 122, App->gui->font2, "MAIN MENU", App->gui->beige, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateLabel(&scene2Labels, LABEL, 50, 22, App->gui->font2, "RESUME", App->gui->beige, (j1UserInterfaceElement*)settings_window);
		
		PlaceEntities(6);
		App->shop->PlaceShopScene2();
		App->entity->CreateEntity(JUDGE, 410, 850);

		startup_time.Start();
		windowTime.Start();
		spawn = false;
	}

	return true;
}

bool j1Scene2::PreUpdate() {

	BROFILER_CATEGORY("Level2PreUpdate", Profiler::Color::Orange)
	App->entity->currentPlayer->current_points.erase();

	return true;
}

bool j1Scene2::Update(float dt) {

	BROFILER_CATEGORY("Level2Update", Profiler::Color::LightSeaGreen)
	iPoint pos = App->map->WorldToMap((int)App->entity->currentPlayer->collider->rect.x + (int)App->entity->currentPlayer->collider->rect.w / 2, (int)App->entity->currentPlayer->collider->rect.y + (int)App->entity->currentPlayer->collider->rect.h);
	if (((pos.x >= 53 && pos.x <= 54 && pos.y == 63) ||
		(pos.x >= 64 && pos.x <= 65 && pos.y >= 67 && pos.y <= 68)) && !spawn){
		spawn = true;
		App->entity->AddEnemy(38, 64, EXODUS);
		App->audio->PlayMusic("audio/music/song021.ogg", 0.0f);
	}

	time_scene2 = startup_time.ReadSec();

	// ---------------------------------------------------------------------------------------------------------------------
	// USER INTERFACE MANAGEMENT
	// ---------------------------------------------------------------------------------------------------------------------

	App->gui->UpdateButtonsState(&scene2Buttons, App->gui->buttonsScale);
	App->gui->UpdateWindow(settings_window, &scene2Buttons, &scene2Labels, &scene2Boxes);
	score_player = App->entity->currentPlayer->coins;
	current_points = std::to_string(score_player);

	bool a = App->scene2->active;
	bool b = a;

	if (App->scene2->active && App->scene2->startup_time.Read() > 1700) {
		if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || closeSettings ||
			(SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_START) == KEY_DOWN && windowTime.Read() >= lastWindowTime + 200)) {
			settings_window->visible = !settings_window->visible;
			App->gamePaused = !App->gamePaused;
			lastWindowTime = windowTime.Read();

			settings_window->position = { App->gui->settingsPosition.x - App->render->camera.x / (int)App->win->GetScale(),
				App->gui->settingsPosition.y - App->render->camera.y / (int)App->win->GetScale() };

			for (std::list<j1Button*>::iterator item = scene2Buttons.begin(); item != scene2Buttons.end(); ++item) {
				if ((*item)->parent == settings_window) {
					(*item)->visible = !(*item)->visible;
					(*item)->position.x = settings_window->position.x + (*item)->initialPosition.x;
					(*item)->position.y = settings_window->position.y + (*item)->initialPosition.y;
				}
			}
			for (std::list<j1Label*>::iterator item = scene2Labels.begin(); item != scene2Labels.end(); ++item) {
				if ((*item)->parent == settings_window) {
					(*item)->visible = !(*item)->visible;
					(*item)->position.x = settings_window->position.x + (*item)->initialPosition.x;
					(*item)->position.y = settings_window->position.y + (*item)->initialPosition.y;
				}
			}
			for (std::list<j1Box*>::iterator item = scene2Boxes.begin(); item != scene2Boxes.end(); ++item) {
				if ((*item)->parent == settings_window) {
					(*item)->visible = !(*item)->visible;
					(*item)->position.x = settings_window->position.x + (*item)->initialPosition.x;
					(*item)->position.y = settings_window->position.y + (*item)->initialPosition.y;

					(*item)->minimum = (*item)->originalMinimum + settings_window->position.x;
					(*item)->maximum = (*item)->originalMaximum + settings_window->position.x;

					(*item)->distanceCalculated = false;
				}
			}

			if (!settings_window->visible) closeSettings = false;
		}
	}

	App->gui->UpdateSliders(&scene2Boxes);

	// Button actions
	for (std::list<j1Button*>::iterator item = scene2Buttons.begin(); item != scene2Buttons.end(); ++item) {
		switch ((*item)->state)
		{
		case IDLE:
			(*item)->situation = (*item)->idle;
			break;

		case HOVERED:
			(*item)->situation = (*item)->hovered;
			break;

		case RELEASED:
			(*item)->situation = (*item)->idle;
			if ((*item)->bfunction == GO_TO_MENU) {
				backToMenu = true;
				App->gamePaused = false;
				settings_window->visible = false;
				App->fade->FadeToBlack();
			}
			else if ((*item)->bfunction == CLOSE_SETTINGS) {
				closeSettings = true;
			}
			else if ((*item)->bfunction == OTHER_LEVEL) {
				changingScene = true;
				App->gamePaused = false;
				settings_window->visible = false;
				App->fade->FadeToBlack();
			}
			else if ((*item)->bfunction == SAVE_GAME) {
				App->SaveGame("save_game.xml");
			}
			else if ((*item)->bfunction == CLOSE_GAME) {
				continueGame = false;
			}
			break;

		case CLICKED:
			(*item)->situation = (*item)->clicked;
			break;
		}
	}

	// Load and Save
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
	{
		App->LoadGame("save_game.xml");
	}

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
		App->entity->currentPlayer->victory = true;

	if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN
		|| (SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_BACK) == KEY_DOWN && statsTime.Read() >= lastStatsTime + 200)) {

		lastStatsTime = statsTime.Read();

		if (profile_active) profile_active = false;
		else profile_active = true;
	}

	if (App->entity->currentPlayer->dead == true) {
		toLoseScene = true;

		App->fade->FadeToBlack();

		if (App->fade->IsFading() == 0)
			ChangeSceneDeath();
	}

	if (App->entity->currentPlayer->victory == true) {

		App->fade->FadeToBlack();

		if (App->fade->IsFading() == 0)
			ChangeSceneVictory();
	}

	if (backToMenu && App->fade->IsFading() == 0)
		ChangeSceneMenu();

	// ---------------------------------------------------------------------------------------------------------------------
	// DRAWING EVERYTHING ON THE SCREEN
	// ---------------------------------------------------------------------------------------------------------------------	
	App->render->Blit(bg, 0, 0, NULL, SDL_FLIP_NONE, false, 0.7f);
	App->gui->UpdateSliders(&scene2Boxes);
	App->map->Draw();
	App->entity->DrawEntityOrder(dt);
	App->render->reOrder();
	App->render->OrderBlit(App->render->OrderToRender);

	if (App->entity->player_type == KNIGHT && App->entity->currentPlayer->active_Q && App->entity->knight != nullptr)
		App->render->Blit(App->entity->knight->shieldTex, App->entity->currentPlayer->position.x + 2, App->entity->currentPlayer->position.y + 8, NULL, SDL_FLIP_NONE, true, 0.13f);

	return true;
}

bool j1Scene2::PostUpdate() {

	if(finishedDialog2)
	App->render->Blit(lvl2_tex, App->win->width - 400, App->win->height - 200, &rect, SDL_FLIP_NONE, false, 0.3333333f);

	return true;
}

bool j1Scene2::Load(pugi::xml_node& node) {
	active = node.child("activated").attribute("value").as_bool();
	finishedDialog2 = node.child("dialogs").attribute("dialog").as_bool();
	potionCounter = node.child("potions").attribute("counter").as_uint();

	return true;
}

bool j1Scene2::Save(pugi::xml_node& node) const 
{
	pugi::xml_node activated = node.append_child("activated");
	activated.append_attribute("value") = active;

	pugi::xml_node dialogs = node.append_child("dialogs");
	dialogs.append_attribute("dialog") = finishedDialog2;

	pugi::xml_node potions = node.append_child("potions");
	potions.append_attribute("counter") = potionCounter;

	return true;
}

void j1Scene2::PlaceEntities(int room) {

	App->entity->AddEnemy(92, 58, TURRET);
	App->entity->AddEnemy(84, 55, TURRET);

		App->entity->AddEnemy(77, 78, FIRE);
	App->entity->AddEnemy(76, 73, FIRE);
	App->entity->AddEnemy(72, 66, FIRE);

	App->entity->AddEnemy(48, 46, SLIME);
	App->entity->AddEnemy(52, 46, SLIME);
	App->entity->AddEnemy(51, 41, FIRE);

	App->entity->AddEnemy(36, 36, TURRET);
	App->entity->AddEnemy(40, 31, TURRET);
	App->entity->AddEnemy(35, 38, FIRE);

	App->entity->AddEnemy(22, 12, SLIME);
	App->entity->AddEnemy(24, 20, SLIME);
	App->entity->AddEnemy(22, 27, SLIME);
	App->entity->AddEnemy(26, 32, SLIME);

	App->entity->AddEnemy(34, 44, TURRET);
	App->entity->AddEnemy(14, 41, FIRE);
	App->entity->AddEnemy(12, 42, FIRE);

	App->entity->AddEnemy(5, 74, TURRET);
	App->entity->AddEnemy(14, 83, SLIME);
	App->entity->AddEnemy(14, 79, SLIME);
	App->entity->AddEnemy(14, 71, SLIME);
	App->entity->AddEnemy(10, 75, FIRE);

	App->entity->AddEnemy(39, 72, TURRET);
	App->entity->AddEnemy(32, 74, TURRET);
	App->entity->AddEnemy(40, 84, TURRET);
	App->entity->AddEnemy(33, 88, SLIME);

	App->entity->AddEnemy(83, 12, SLIME);
	App->entity->AddEnemy(91, 17, SLIME);
	App->entity->AddEnemy(87, 29, FIRE);

	App->entity->AddEnemy(68, 8, TURRET);
	App->entity->AddEnemy(63, 10, TURRET);
	App->entity->AddEnemy(67, 23, TURRET); 
	App->entity->AddEnemy(71, 22, TURRET);

	App->entity->AddEnemy(51, 25, FIRE);
	App->entity->AddEnemy(49, 18, FIRE);
	App->entity->AddEnemy(42, 18, FIRE);
	App->entity->AddEnemy(46, 12, FIRE);
}

bool j1Scene2::CleanUp() {

	LOG("Freeing scene2");
	App->tex->UnLoad(bg);
	App->tex->UnLoad(lvl2_tex);
	App->tex->UnLoad(gui_tex);
	App->tex->UnLoad(debug_tex);
	App->map->CleanUp();
	App->collisions->CleanUp();
	App->tex->CleanUp();
	App->entity->DestroyEntities();
	App->gui->CleanUp();
	App->particles->CleanUp();

	potionCounter = 0;
	finishedDialog2 = false;

	if (App->entity->knight) App->entity->knight->CleanUp();
	if (App->entity->mage) App->entity->mage->CleanUp();

	for (std::list<j1Button*>::iterator item = scene2Buttons.begin(); item != scene2Buttons.end(); ++item) {
		(*item)->CleanUp();
		scene2Buttons.remove(*item);
	}

	for (std::list<j1Label*>::iterator item = scene2Labels.begin(); item != scene2Labels.end(); ++item) {
		(*item)->CleanUp();
		scene2Labels.remove(*item);
	}

	for (std::list<j1Box*>::iterator item = scene2Boxes.begin(); item != scene2Boxes.end(); ++item) {
		(*item)->CleanUp();
		scene2Boxes.remove(*item);
	}

	delete settings_window;
	if (settings_window != nullptr) settings_window = nullptr;

	App->path->CleanUp();
	App->shop->restartingShop = true;
	App->shop->CleanUp();
	App->entity->CleanUp();
	App->dialog->active = false;
	App->dialog->CleanUp();

	return true;
}

void j1Scene2::ChangeSceneMenu()
{
	App->scene2->active = false;
	App->menu->active = true;
	changingScene = false;

	CleanUp();
	App->entity->active = false;
	App->menu->Start();
	App->render->camera = { 0,0 };
	App->entity->player_type = NO_PLAYER;
	backToMenu = false;
}

void j1Scene2::ChangeSceneDeath() {
	App->scene2->active = false;
	App->lose->active = true;
	App->dialog->CleanUp();

	CleanUp();
	App->entity->active = false;
	App->lose->Start();
	App->render->camera = { 0,0 };
	toLoseScene = false;
}

void j1Scene2::ChangeSceneVictory() {
	App->scene2->active = false;
	App->victory->active = true;
	App->dialog->CleanUp();

	CleanUp();
	App->shop->CleanUp();
	App->entity->active = false;
	App->render->camera = { 0,0 };

	App->victory->Start();
	App->entity->currentPlayer->victory = false;
}