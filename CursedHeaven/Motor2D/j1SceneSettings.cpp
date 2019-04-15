#include "j1SceneSettings.h"
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

#include "Brofiler/Brofiler.h"

j1SceneSettings::j1SceneSettings()
{
}

j1SceneSettings::~j1SceneSettings() {}

bool j1SceneSettings::Awake(pugi::xml_node &)
{
	LOG("Loading Settings");
	bool ret = true;

	if (App->menu->active == true)
		active = false;

	if (active == false)
		LOG("Settings not active.");

	if (App->menu->active == false)
	{
		LOG("Unable to load the settings.");
		ret = false;
	}

	return ret;
}

bool j1SceneSettings::Start()
{

	if (active)
	{
		// The map is loaded
		App->map->Load("menu.tmx");

		// The audio is played
		App->audio->PlayMusic("audio/music/credits_music.ogg", 1.0f);

		// Loading textures
		wind_tex = App->tex->Load("gui/uipack_rpg_sheet_2.png");
		gui_tex2 = App->tex->Load("gui/uipack_rpg_sheet.png");

		// Loading fonts
		font3 = App->font->Load("fonts/PixelCowboy/PixelCowboy.otf", 8);

		// Creates the window
		sets_window = App->gui->CreateBox(&settingBoxes, BOX, 60, 55, { 621, 377, 785, 568 }, wind_tex);
		sets_window->visible = true;

		// Sliders
		App->gui->CreateBox(&settingBoxes, BOX, App->gui->lastSlider1X, App->gui->slider1Y, { 388, 455, 28, 42 }, gui_tex2, (j1UserInterfaceElement*)sets_window, App->gui->minimum, App->gui->maximum);
		App->gui->CreateBox(&settingBoxes, BOX, App->gui->lastSlider2X, App->gui->slider2Y, { 388, 455, 28, 42 }, gui_tex2, (j1UserInterfaceElement*)sets_window, App->gui->minimum, App->gui->maximum);

		// Go Back Button
		SDL_Rect idle5 = { 382, 508, 37, 36 };
		SDL_Rect hovered5 = { 343, 508, 37, 36 };
		SDL_Rect clicked5 = { 421, 511, 37, 33 };
		App->gui->CreateButton(&settingButtons, BUTTON, 7, 7, idle5, hovered5, clicked5, gui_tex2, GO_TO_MENU);

		App->gui->CreateLabel(&settingLabels, LABEL, 132, 63, font3, "Settings", App->gui->brown, (j1UserInterfaceElement*)sets_window);

		App->gui->CreateLabel(&settingLabels, LABEL, 95, 101, font3, "Sound", App->gui->brown, (j1UserInterfaceElement*)sets_window);

		App->gui->CreateLabel(&settingLabels, LABEL, 96, 135, font3, "Music", App->gui->brown, (j1UserInterfaceElement*)sets_window);

		startup_time.Start();
	}

	return true;
}

bool j1SceneSettings::PreUpdate()
{
	BROFILER_CATEGORY("CreditsPreUpdate", Profiler::Color::Orange)
		return true;
}

bool j1SceneSettings::Update(float dt)
{
	BROFILER_CATEGORY("CreditsUpdate", Profiler::Color::LightSeaGreen)

		// ---------------------------------------------------------------------------------------------------------------------
		// USER INTERFACE MANAGEMENT
		// ---------------------------------------------------------------------------------------------------------------------	
	
	App->gui->UpdateSliders(&settingBoxes);
	App->gui->UpdateButtonsState(&settingButtons, App->gui->buttonsScale);
	//App->gui->UpdateWindow(sets_window, &setsButtons, &setsLabels, &setsBoxes);

	// Button actions
	for (std::list<j1Button*>::iterator item = settingButtons.begin(); item != settingButtons.end(); ++item) {
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
			(*item)->situation = (*item)->idle;
			backToMenu = true;
			App->fade->FadeToBlack();
			break;

		case CLICKED:
			(*item)->situation = (*item)->clicked;
			break;
		}
		
	}

	// Managing scene transitions
	if (backToMenu && App->fade->IsFading() == 0) {
		ChangeScene();
	}

	// ---------------------------------------------------------------------------------------------------------------------
	// DRAWING EVERYTHING ON THE SCREEN
	// ---------------------------------------------------------------------------------------------------------------------	

	// Blitting background and animations
	App->map->Draw();


	// Blitting the buttons and labels of the menu
	for (std::list<j1Button*>::iterator item = settingButtons.begin(); item != settingButtons.end(); ++item) {
		(*item)->Draw(App->gui->buttonsScale);
	}
	for (std::list<j1Label*>::iterator item = settingLabels.begin(); item != settingLabels.end(); ++item) {
		(*item)->Draw();
	}

	// Blitting settings window
	if (sets_window != nullptr && sets_window->visible == true)
	{
		sets_window->Draw(App->gui->settingsWindowScale);
	}

	// Blitting the buttons, labels and boxes (sliders) of the window
	for (std::list<j1Button*>::iterator item = settingButtons.begin(); item != settingButtons.end(); ++item) {
		if ((*item)->parent == nullptr) continue;

		if ((*item)->parent->visible == false)
			(*item)->visible = false;
		else
			(*item)->Draw(App->gui->buttonsScale);

	}

	for (std::list<j1Box*>::iterator item = settingBoxes.begin(); item != settingBoxes.end(); ++item) {
		if ((*item)->parent == nullptr) continue;

		if ((*item)->parent->visible == false)
			(*item)->visible = false;
		else
			(*item)->Draw(App->gui->buttonsScale);
	}

	for (std::list<j1Label*>::iterator item = settingLabels.begin(); item != settingLabels.end(); ++item) {
		if ((*item)->parent == nullptr) continue;

		if ((*item)->parent->visible == false)
			(*item)->visible = false;
		else {
			if ((*item)->text == "Settings")
				(*item)->Draw();
			else
				(*item)->Draw(0.6f);
		}
	}

	return true;
}

bool j1SceneSettings::PostUpdate()
{
	BROFILER_CATEGORY("CreditsPostUpdate", Profiler::Color::Yellow)

		if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
			continueGame = false;

	return continueGame;
}

bool j1SceneSettings::CleanUp()
{
	LOG("Freeing all textures");
	App->tex->UnLoad(gui_tex2);
	App->tex->UnLoad(wind_tex);

	App->map->CleanUp();
	App->tex->CleanUp();

	for (std::list<j1Button*>::iterator item = settingButtons.begin(); item != settingButtons.end(); ++item) {
		(*item)->CleanUp();
		settingButtons.remove(*item);
	}

	for (std::list<j1Label*>::iterator item = settingLabels.begin(); item != settingLabels.end(); ++item) {
		(*item)->CleanUp();
		settingLabels.remove(*item);
	}

	for (std::list<j1Box*>::iterator item = settingBoxes.begin(); item != settingBoxes.end(); ++item) {
		(*item)->CleanUp();
		settingBoxes.remove(*item);
	}

	delete sets_window;
	if (sets_window != nullptr) sets_window = nullptr;

	return true;
}

void j1SceneSettings::ChangeScene()
{
	this->active = false;
	backToMenu = false;
	CleanUp();
	App->menu->active = true;
	App->menu->Start();
}