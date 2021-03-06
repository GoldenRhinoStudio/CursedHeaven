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
#include "j1Shop.h"
#include "j1Minimap.h"
#include "j1TransitionManager.h"
#include "j1SceneKeyConfig.h"
#include "j1Video.h"

#include "Brofiler/Brofiler.h"

j1Scene1::j1Scene1() : j1Module()
{
	name.assign("scene1");
}

// Destructor
j1Scene1::~j1Scene1()
{}

// Called before render is available
bool j1Scene1::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene 1");
	bool ret = true;
	if (App->video->active == true)
		active = false;

	if (App->menu->active == true)
		active = false;

	if (active == false)
		LOG("Scene1 not active.");

	return ret;
}

// Called before the first frame
bool j1Scene1::Start()
{
	if (active)
	{
		App->map->draw_with_quadtrees = true;
		// The map is loaded
		if (App->map->Load("greenmount.tmx"))
		{
			int w, h;
			uchar* data = NULL;
			if (App->map->CreateWalkabilityMap(w, h, &data))
			{
				App->path->SetMap(w, h, data);
			}

			RELEASE_ARRAY(data);
		}

		// The audio is played	
		App->audio->PlayMusic("audio/music/song034.ogg", 1.0f);

		// Textures are loaded
		debug_tex = App->tex->Load("maps/path2.png");
		gui_tex = App->tex->Load("gui/uipack_rpg_sheet.png");
		lvl1_tex = App->tex->Load("maps/minimap_lvl1.png");
		bg = App->tex->Load("maps/Background.png");

		// Creating UI
		SDL_Rect section = { 9,460,315,402 };
		App->scene1->settings_window = App->gui->CreateBox(&scene1Boxes, BOX, App->gui->settingsPosition.x, App->gui->settingsPosition.y, section, gui_tex);
		App->scene1->settings_window->visible = false;

		SDL_Rect idle = { 631, 12, 151, 38 };
		SDL_Rect hovered = { 963, 12, 151, 38 };
		SDL_Rect clicked = { 797, 14, 151, 37 };

		SDL_Rect idle2 = { 1181, 12, 99, 38 };
		SDL_Rect hovered2 = { 1386, 12, 99, 38 };
		SDL_Rect clicked2 = { 1283, 12, 99, 38 };

		SDL_Rect slider_r = { 860,334,180,5 };
		App->gui->CreateButton(&scene1Buttons, BUTTON, 20, 65, slider_r, slider_r, slider_r, gui_tex, NO_FUNCTION, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateButton(&scene1Buttons, BUTTON, 20, 100, slider_r, slider_r, slider_r, gui_tex, NO_FUNCTION, (j1UserInterfaceElement*)settings_window);

		App->gui->CreateBox(&scene1Boxes, BOX, 50, 55, { 388, 455, 28, 42 }, gui_tex, (j1UserInterfaceElement*)settings_window, 20, 92);
		App->gui->CreateBox(&scene1Boxes, BOX, 50, 90, { 388, 455, 28, 42 }, gui_tex, (j1UserInterfaceElement*)settings_window, 20, 92);

		App->gui->CreateButton(&scene1Buttons, BUTTON, 10, 20, idle2, hovered2, clicked2, gui_tex, CLOSE_SETTINGS, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateButton(&scene1Buttons, BUTTON, 70, 20, idle2, hovered2, clicked2, gui_tex, SAVE_GAME, (j1UserInterfaceElement*)settings_window);

		App->gui->CreateButton(&scene1Buttons, BUTTON, 30, 120, idle, hovered, clicked, gui_tex, GO_TO_MENU, (j1UserInterfaceElement*)settings_window);

		App->gui->CreateLabel(&scene1Labels, LABEL, 25, 40, App->gui->font2, "SOUND", App->gui->brown, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateLabel(&scene1Labels, LABEL, 25, 75, App->gui->font2, "MUSIC", App->gui->brown, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateLabel(&scene1Labels, LABEL, 48, 122, App->gui->font2, "MAIN MENU", App->gui->beige, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateLabel(&scene1Labels, LABEL, 20, 22, App->gui->font2, "RESUME", App->gui->beige, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateLabel(&scene1Labels, LABEL, 83, 22, App->gui->font2, "SAVE", App->gui->beige, (j1UserInterfaceElement*)settings_window);

		PlaceEntities(6);
		App->shop->PlaceShopScene1();
		App->entity->CreateEntity(JUDGE, 210, 760);

		startup_time.Start();
		windowTime.Start();
	}

	return true;
}

// Called each loop iteration
bool j1Scene1::PreUpdate()
{
	BROFILER_CATEGORY("Level1PreUpdate", Profiler::Color::Orange)
	current_points.erase();

	return true;
}

// Called each loop iteration
bool j1Scene1::Update(float dt)
{
	BROFILER_CATEGORY("Level1Update", Profiler::Color::LightSeaGreen)

	time_scene1 = startup_time.ReadSec();

	// ---------------------------------------------------------------------------------------------------------------------
	// USER INTERFACE MANAGEMENT
	// ---------------------------------------------------------------------------------------------------------------------

	App->gui->UpdateButtonsState(&scene1Buttons, App->gui->buttonsScale);
	App->gui->UpdateWindow(settings_window, &scene1Buttons, &scene1Labels, &scene1Boxes);
	score_player = App->entity->currentPlayer->coins;
	current_points = std::to_string(score_player);

	if (App->scene1->active && App->scene1->startup_time.Read() > 1700) {
		if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || closeSettings ||
			(SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_START) == KEY_DOWN && windowTime.Read() >= lastWindowTime + 200)) {
			settings_window->visible = !settings_window->visible;
			App->gamePaused = !App->gamePaused;
			lastWindowTime = windowTime.Read();

			settings_window->position = { App->gui->settingsPosition.x - App->render->camera.x / (int)App->win->GetScale(),
				App->gui->settingsPosition.y - App->render->camera.y / (int)App->win->GetScale() };

			for (std::list<j1Button*>::iterator item = scene1Buttons.begin(); item != scene1Buttons.end(); ++item) {
				if ((*item)->parent == settings_window) {
					(*item)->visible = !(*item)->visible;
					(*item)->position.x = settings_window->position.x + (*item)->initialPosition.x;
					(*item)->position.y = settings_window->position.y + (*item)->initialPosition.y;
				}
			}
			for (std::list<j1Label*>::iterator item = scene1Labels.begin(); item != scene1Labels.end(); ++item) {
				if ((*item)->parent == settings_window) {
					(*item)->visible = !(*item)->visible;
					(*item)->position.x = settings_window->position.x + (*item)->initialPosition.x;
					(*item)->position.y = settings_window->position.y + (*item)->initialPosition.y;
				}
			}
			for (std::list<j1Box*>::iterator item = scene1Boxes.begin(); item != scene1Boxes.end(); ++item) {
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

	App->gui->UpdateSliders(&scene1Boxes);

	// Save

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN || mustSave) {
		App->SaveGame("save_game.xml");
		mustSave = false;
	}

	// Button actions
	for (std::list<j1Button*>::iterator item = scene1Buttons.begin(); item != scene1Buttons.end(); ++item) {
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
				mustSave = true;
				closeSettings = true;
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

	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
		App->entity->currentPlayer->victory = true;

	// Managing scene transitions
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN || changingScene) {
		changingScene = true;

		App->fade->FadeToBlack();

		if (App->fade->IsFading() == 0)
			ChangeSceneMenu();
	}

	if (App->input->GetKey(App->key_config->TABULADOR) == KEY_DOWN
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
			ChangeScene2();
	}

	if (backToMenu && App->fade->IsFading() == 0)
		ChangeSceneMenu();

	// ---------------------------------------------------------------------------------------------------------------------
	// DRAWING EVERYTHING ON THE SCREEN
	// ---------------------------------------------------------------------------------------------------------------------	
	App->render->Blit(bg, 0, 0, NULL, SDL_FLIP_NONE, false, 0.7f);
	App->map->Draw();
	App->entity->DrawEntityOrder(dt);
	App->render->reOrder();
	App->render->OrderBlit(App->render->OrderToRender);

	if (App->entity->player_type == KNIGHT && App->entity->currentPlayer->active_Q && App->entity->knight != nullptr)
		App->render->Blit(App->entity->knight->shieldTex, App->entity->currentPlayer->position.x + 2, App->entity->currentPlayer->position.y + 8, NULL, SDL_FLIP_NONE, true, 0.13f);

	return true;
}

// Called each loop iteration
bool j1Scene1::PostUpdate()
{
	BROFILER_CATEGORY("Level1PostUpdate", Profiler::Color::Yellow)

	if (finishedDialog)
		App->render->Blit(lvl1_tex, App->win->width - 400, App->win->height - 200, &rect, SDL_FLIP_NONE, false, 0.3333333f);

	return continueGame;
}

bool j1Scene1::Load(pugi::xml_node& node) {
	active = node.child("activated").attribute("value").as_bool();
	finishedDialog = node.child("dialogs").attribute("dialog").as_bool();
	ableSellerDialog = node.child("dialogs").attribute("sellerDialog").as_bool();
	potionCounter = node.child("potions").attribute("counter").as_uint();

	return true;
}

bool j1Scene1::Save(pugi::xml_node& node) const
{
	pugi::xml_node activated = node.append_child("activated");
	activated.append_attribute("value") = active;
	
	pugi::xml_node dialogs = node.append_child("dialogs");
	dialogs.append_attribute("dialog") = finishedDialog;
	dialogs.append_attribute("sellerDialog") = ableSellerDialog;

	pugi::xml_node potions = node.append_child("potions");
	potions.append_attribute("counter") = potionCounter;

	return true;
}

void j1Scene1::PlaceEntities(int room)
{
	App->entity->AddEnemy(6, 57, SLIME);
	App->entity->AddEnemy(15, 54, SLIME);
	App->entity->AddEnemy(17, 61, SLIME);

	App->entity->AddEnemy(31, 65, SLIME);
	App->entity->AddEnemy(28, 65, SLIME);
	App->entity->AddEnemy(28, 53, SLIME);

	App->entity->AddEnemy(29, 40, SLIME);
	App->entity->AddEnemy(33, 41, SLIME);
	App->entity->AddEnemy(14, 41, SLIME);

	App->entity->AddEnemy(45, 45, SLIME);
	App->entity->AddEnemy(43, 39, SLIME);
	App->entity->AddEnemy(38, 41, SLIME);

	App->entity->AddEnemy(29, 19, SLIME);
	App->entity->AddEnemy(28, 22, SLIME);
	App->entity->AddEnemy(26, 26, SLIME);

	App->entity->AddEnemy(46, 25, SLIME);
	App->entity->AddEnemy(45, 32, SLIME);
	App->entity->AddEnemy(38, 28, SLIME);

	App->entity->AddEnemy(23, 4, SLIME);
	App->entity->AddEnemy(12, 4, SLIME);
	App->entity->AddEnemy(17, 13, SLIME);

	App->entity->AddEnemy(49, 4, SLIME);
	App->entity->AddEnemy(60, 7, SLIME);
	App->entity->AddEnemy(59, 11, SLIME);
	App->entity->AddEnemy(70, 8, SLIME);

	App->entity->AddEnemy(85, 23, SLIME);
	App->entity->AddEnemy(80, 19, SLIME);
	App->entity->AddEnemy(70, 25, SLIME);

	App->entity->AddEnemy(88, 46, SLIME);
	App->entity->AddEnemy(80, 42, SLIME);
	App->entity->AddEnemy(85, 60, SLIME);
	App->entity->AddEnemy(80, 65, SLIME);

	App->entity->AddEnemy(53, 68, MINDFLYER);
}

// Called before quitting
bool j1Scene1::CleanUp()
{
	LOG("Freeing scene");
	App->tex->UnLoad(bg);
	App->tex->UnLoad(lvl1_tex);
	App->tex->UnLoad(gui_tex);
	App->tex->UnLoad(debug_tex);
	App->map->CleanUp();
	App->collisions->CleanUp();
	App->tex->CleanUp();
	App->entity->DestroyEntities();
	App->gui->CleanUp();
	App->particles->CleanUp();

	potionCounter = 0;
	finishedDialog = false;
	ableSellerDialog = true;

	if (App->entity->knight) App->entity->knight->CleanUp();
	if (App->entity->mage) App->entity->mage->CleanUp();

	for (std::list<j1Button*>::iterator item = scene1Buttons.begin(); item != scene1Buttons.end();) {
		(*item)->CleanUp();
		scene1Buttons.erase(item++);
	}

	for (std::list<j1Label*>::iterator item = scene1Labels.begin(); item != scene1Labels.end();) {
		(*item)->CleanUp();
		scene1Labels.erase(item++);
	}

	for (std::list<j1Box*>::iterator item = scene1Boxes.begin(); item != scene1Boxes.end();) {
		(*item)->CleanUp();
		scene1Boxes.erase(item++);
	}

	delete settings_window;
	if (settings_window != nullptr) settings_window = nullptr;

	App->path->CleanUp();
	App->entity->CleanUp();
	App->dialog->active = false;
	App->dialog->CleanUp();

	return true;
}

void j1Scene1::ChangeSceneMenu()
{
	App->scene1->active = false;
	App->menu->active = true;
	changingScene = false;

	CleanUp();
	App->shop->restartingShop = true;
	App->shop->CleanUp();
	App->entity->active = false;
	App->menu->Start();
	App->render->camera = { 0,0 }; 
	App->entity->player_type = NO_PLAYER;
	backToMenu = false;
}

void j1Scene1::ChangeSceneDeath() {
	App->scene1->active = false;
	App->lose->active = true;

	CleanUp();
	App->shop->restartingShop = true;
	App->shop->CleanUp();
	App->entity->active = false;
	App->lose->Start();
	App->render->camera = { 0,0 }; 
	toLoseScene = false;
}

void j1Scene1::ChangeScene2() {
	App->scene1->active = false;
	App->scene2->active = true;

	CleanUp();
	App->shop->CleanUp();
	App->entity->active = false;
	App->scene2->Start();

	App->gui->Start();
	App->entity->active = true;
	App->entity->CreatePlayer2();
	App->entity->Start();
	App->particles->Start();

	App->entity->currentPlayer->victory = false;
}