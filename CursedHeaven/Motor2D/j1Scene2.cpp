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
#include "j1Hook.h"
#include "j1Player.h"
#include "j1SceneMenu.h"
#include "j1Scene2.h"
#include "j1Scene1.h"
#include "j1FadeToBlack.h"
#include "j1Pathfinding.h"
#include "j1Coin.h"
#include "j1Fonts.h"
#include "j1Label.h"
#include "j1Box.h"

#include "Brofiler/Brofiler.h"

j1Scene2::j1Scene2() : j1Module()
{
	name.create("scene2");
	coin_hud.LoadEnemyAnimations("idle", "coin");
}

// Destructor
j1Scene2::~j1Scene2()
{}

// Called before render is available
bool j1Scene2::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene 2");
	bool ret = true;

	if (App->menu->active || App->scene1->active)
		active = false;

	if (active == false)
		LOG("Scene 2 not active");

	// Copying the position of the player
	initialScene2Position.x = config.child("initialPlayerPosition").attribute("x").as_int();
	initialScene2Position.y = config.child("initialPlayerPosition").attribute("y").as_int();

	if (App->scene1->active == true)
		active = false;

	return ret;
}

// Called before the first frame
bool j1Scene2::Start()
{
	if (active)
	{
		// The map is loaded
		if (App->map->Load("lvl2.tmx"))
		{
			int w, h;
			uchar* data = NULL;
			if (App->map->CreateWalkabilityMap(w, h, &data))
			{
				App->path->SetMap(w, h, data);
			}

			RELEASE_ARRAY(data);
		}

		// Textures are loaded
		debug_tex = App->tex->Load("maps/path2.png");
		gui_tex = App->tex->Load("gui/atlas.png");
		coin_tex = App->tex->Load("textures/coin.png");

		// The audio is played
		App->audio->PlayMusic("audio/music/level1_music.ogg", 1.0f);

		// Loading fonts
		font = App->font->Load("fonts/PixelCowboy/PixelCowboy.otf", 8);


		// Creating UI
		SDL_Rect section = { 537, 0, 663, 712 };
		settings_window = App->gui->CreateBox(&scene2Boxes, BOX, App->gui->settingsPosition.x, App->gui->settingsPosition.y, section, gui_tex);
		settings_window->visible = false;

		SDL_Rect idle = { 0, 391, 84, 49 } ;
		SDL_Rect hovered = { 0, 293, 84, 49 };
		SDL_Rect clicked = { 0, 342, 84, 49 };
		App->gui->CreateButton(&scene2Buttons, BUTTON, 31, 105, idle, hovered, clicked, gui_tex, SAVE_GAME, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateButton(&scene2Buttons, BUTTON, 78, 105, idle, hovered, clicked, gui_tex, CLOSE_GAME, (j1UserInterfaceElement*)settings_window);

		App->gui->CreateBox(&scene2Boxes, BOX, App->gui->lastSlider1X, App->gui->slider1Y, { 416, 72, 28, 42 }, gui_tex, (j1UserInterfaceElement*)settings_window, App->gui->minimum, App->gui->maximum);
		App->gui->CreateBox(&scene2Boxes, BOX, App->gui->lastSlider2X, App->gui->slider2Y, { 416, 72, 28, 42 }, gui_tex, (j1UserInterfaceElement*)settings_window, App->gui->minimum, App->gui->maximum);

		SDL_Rect idle2 = { 28, 201, 49, 49 };
		SDL_Rect hovered2 = { 77, 201, 49, 49 };
		SDL_Rect clicked2 = { 126, 201, 49, 49 };
		App->gui->CreateButton(&scene2Buttons, BUTTON, 63, 135, idle2, hovered2, clicked2, gui_tex, CLOSE_SETTINGS, (j1UserInterfaceElement*)settings_window);

		SDL_Rect idle3 = { 312, 292, 49, 49 };
		SDL_Rect hovered3 = { 361, 292, 49, 49 };
		SDL_Rect clicked3 = { 310, 400, 49, 49 };
		App->gui->CreateButton(&scene2Buttons, BUTTON, 89, 135, idle3, hovered3, clicked3, gui_tex, OTHER_LEVEL, (j1UserInterfaceElement*)settings_window);

		SDL_Rect idle4 = { 417, 292, 49, 49 };
		SDL_Rect hovered4 = { 417, 345, 49, 49 };
		SDL_Rect clicked4 = { 417, 400, 49, 49 };
		App->gui->CreateButton(&scene2Buttons, BUTTON, 37, 135, idle4, hovered4, clicked4, gui_tex, GO_TO_MENU, (j1UserInterfaceElement*)settings_window);

		App->gui->CreateLabel(&scene2Labels, LABEL, 44, 9, font, "Settings", App->gui->brown, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateLabel(&scene2Labels, LABEL, 30, 50, font, "Sound", App->gui->brown, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateLabel(&scene2Labels, LABEL, 30, 89, font, "Music", App->gui->brown, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateLabel(&scene2Labels, LABEL, 38, 143, font, "Menu", App->gui->grey, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateLabel(&scene2Labels, LABEL, 33, 110, font, "Save", App->gui->beige, (j1UserInterfaceElement*)settings_window);
		App->gui->CreateLabel(&scene2Labels, LABEL, 81, 110, font, "Quit", App->gui->beige, (j1UserInterfaceElement*)settings_window);

		PlaceEntities();

		startup_time.Start();
	}

	return true;
}

// Called each loop iteration
bool j1Scene2::PreUpdate()
{
	BROFILER_CATEGORY("Level2PreUpdate", Profiler::Color::Orange)

	return true;
}

// Called each loop iteration
bool j1Scene2::Update(float dt)
{
	BROFILER_CATEGORY("Level2Update", Profiler::Color::LightSeaGreen)

		time_scene2 = startup_time.ReadSec();

	// ---------------------------------------------------------------------------------------------------------------------
	// USER INTERFACE MANAGEMENT
	// ---------------------------------------------------------------------------------------------------------------------	

	App->gui->UpdateButtonsState(&scene2Buttons);
	App->gui->UpdateWindow(settings_window, &scene2Buttons, &scene2Labels, &scene2Boxes);
	App->gui->UpdateSliders(&scene2Boxes);

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN || closeSettings) {
		settings_window->visible = !settings_window->visible;
		App->gamePaused = !App->gamePaused;

		if (App->render->camera.x != 0)
			settings_window->position = { (int)App->entity->player->position.x - App->gui->settingsPosition.x, App->gui->settingsPosition.y };

		for (p2List_item<j1Button*>* item = scene2Buttons.start; item != nullptr; item = item->next) {
			if (item->data->parent == settings_window) {
				item->data->visible = !item->data->visible;
				item->data->position.x = settings_window->position.x + item->data->initialPosition.x;
				item->data->position.y = settings_window->position.y + item->data->initialPosition.y;
			}
		}
		for (p2List_item<j1Label*>* item = scene2Labels.start; item != nullptr; item = item->next) {
			if (item->data->parent == settings_window) {
				item->data->visible = !item->data->visible;
				item->data->position.x = settings_window->position.x + item->data->initialPosition.x;
				item->data->position.y = settings_window->position.y + item->data->initialPosition.y;
			}
		}
		for (p2List_item<j1Box*>* item = scene2Boxes.start; item != nullptr; item = item->next) {
			if (item->data->parent == settings_window) {
				item->data->visible = !item->data->visible;
				item->data->position.x = settings_window->position.x + item->data->initialPosition.x;
				item->data->position.y = settings_window->position.y + item->data->initialPosition.y;

				item->data->minimum = item->data->originalMinimum + settings_window->position.x;
				item->data->maximum = item->data->originalMaximum + settings_window->position.x;

				item->data->distanceCalculated = false;
			}
		}

		if (!settings_window->visible) closeSettings = false;
	}

	// Button actions
	for (p2List_item<j1Button*>* item = scene2Buttons.start; item != nullptr; item = item->next) {
		switch (item->data->state)
		{
		case IDLE:
			item->data->situation = item->data->idle;
			break;

		case HOVERED:
			item->data->situation = item->data->hovered;
			break;

		case RELEASED:
			item->data->situation = item->data->idle;
			if (item->data->bfunction == GO_TO_MENU) {
				backToMenu = true;
				App->gamePaused = false;
				settings_window->visible = false;
				App->fade->FadeToBlack();
			}
			else if (item->data->bfunction == CLOSE_SETTINGS) {
				closeSettings = true;
			}
			else if (item->data->bfunction == OTHER_LEVEL) {
				changingScene = true;
				App->gamePaused = false;
				settings_window->visible = false;
				App->fade->FadeToBlack();
			}
			else if (item->data->bfunction == SAVE_GAME) {
				App->SaveGame("save_game.xml");
			}
			else if (item->data->bfunction == CLOSE_GAME) {
				continueGame = false;
			}
			break;

		case CLICKED:
			item->data->situation = item->data->clicked;
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

	// Managing scene transitions
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN || changingScene) {
		changingScene = true;

		App->fade->FadeToBlack();

		if (App->fade->IsFading() == 0)
			ChangeScene();
	}

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN || resettingLevel)
	{
		resettingLevel = true;
		App->fade->FadeToBlack();

		if (App->fade->IsFading() == 0) {
			App->entity->player->position = initialScene2Position;
			App->render->camera.x = 0;
			App->entity->player->facingRight = true;
			resettingLevel = false;
		}
	}

	if (backToMenu && App->fade->IsFading() == 0) {
		ChangeSceneMenu();
	}

	// ---------------------------------------------------------------------------------------------------------------------
	// DRAWING EVERYTHING ON THE SCREEN
	// ---------------------------------------------------------------------------------------------------------------------	

	App->map->Draw();

	// Blitting settings window
	if (settings_window != nullptr && settings_window->visible == true)
		settings_window->Draw(App->gui->settingsWindowScale);

	SDL_Rect r = coin_hud.GetCurrentFrame(dt);
	App->render->Blit(coin_tex, 3, 700, &r, SDL_FLIP_NONE, 1.0f, 1, 0, INT_MAX, INT_MAX, false);

	if (App->collisions->debug) {

		const p2DynArray<iPoint>* path = App->path->GetLastPath();

		for (uint i = 0; i < path->Count(); ++i)
		{
			iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
			App->render->Blit(debug_tex, pos.x, pos.y);
		}
	}

	return true;
}

// Called each loop iteration
bool j1Scene2::PostUpdate()
{
	BROFILER_CATEGORY("Level2PostUpdate", Profiler::Color::Yellow)

	return continueGame;
}

bool j1Scene2::Load(pugi::xml_node& node)
{
	pugi::xml_node activated = node.child("activated");

	bool scene_activated = activated.attribute("true").as_bool();

	if ((scene_activated == false) && active)
		ChangeScene();

	return true;
}

bool j1Scene2::Save(pugi::xml_node& node) const
{
	pugi::xml_node activated = node.append_child("activated");

	activated.append_attribute("true") = active;

	return true;
}

// Called before quitting
bool j1Scene2::CleanUp()
{
	LOG("Freeing scene");
	App->tex->UnLoad(gui_tex);
	App->tex->UnLoad(debug_tex);

	App->map->CleanUp();
	App->collisions->CleanUp();
	App->tex->CleanUp();
	App->entity->DestroyEntities();
	App->gui->CleanUp();

	if (App->entity->player)
		App->entity->player->CleanUp();
	if (App->entity->hook)
		App->entity->hook->CleanUp();

	for (p2List_item<j1Button*>* item = scene2Buttons.start; item != nullptr; item = item->next) {
		item->data->CleanUp();
		scene2Buttons.del(item);
	}

	for (p2List_item<j1Label*>* item = scene2Labels.start; item != nullptr; item = item->next) {
		item->data->CleanUp();
		scene2Labels.del(item);
	}

	for (p2List_item<j1Box*>* item = scene2Boxes.start; item != nullptr; item = item->next) {
		item->data->CleanUp();
		scene2Boxes.del(item);
	}

	delete settings_window;
	if (settings_window != nullptr) settings_window = nullptr;

	App->path->CleanUp();

	return true;
}

void j1Scene2::PlaceEntities()
{
	p2SString folder;
	pugi::xml_document	map_file;

	p2SString tmp("%s%s", folder.GetString(), "maps/lvl2.tmx");

	pugi::xml_parse_result result = map_file.load_file(tmp.GetString());

	pugi::xml_node obj;
	pugi::xml_node group;

	const char* object_name;

	for (group = map_file.child("map").child("objectgroup"); group; group = group.next_sibling("objectgroup"))
	{
		object_name = group.attribute("name").as_string();

		for (obj = group.child("object"); obj; obj = obj.next_sibling("object"))
		{
			if (strcmp(object_name, "coins") == 0)
				App->entity->AddEnemy(obj.attribute("x").as_int(), obj.attribute("y").as_int(), COIN);
			else if (strcmp(object_name, "skeleton") == 0)
				App->entity->AddEnemy(obj.attribute("x").as_int(), obj.attribute("y").as_int(), SKELETON);
			else if (strcmp(object_name, "harpy") == 0)
				App->entity->AddEnemy(obj.attribute("x").as_int(), obj.attribute("y").as_int(), HARPY);
		}
	}
}

void j1Scene2::ChangeScene()
{
	App->scene1->active = true;
	App->scene2->active = false;
	CleanUp();
	App->entity->Start();
	App->scene1->Start();
	App->render->camera = { 0,0 };
	App->path->Start();
	App->entity->player->initialPosition = App->scene1->initialScene1Position;
	App->entity->player->position = App->scene1->initialScene1Position;
	changingScene = false;
}


void j1Scene2::ChangeSceneMenu()
{
	App->scene2->active = false;
	App->menu->active = true;

	CleanUp();
	App->entity->CleanUp();
	App->entity->active = false;
	App->menu->Start();
	App->fade->FadeToBlack();
	App->render->camera = { 0,0 };
	backToMenu = false;
}
