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
	LOG("Loading Credits");
	bool ret = true;

	if (App->menu->active == true)
		active = false;

	if (active == false)
		LOG("Scene1 not active.");

	if (App->menu->active == false)
	{
		LOG("Unable to load the credits.");
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
		gui_tex = App->tex->Load("gui/atlas.png");
		gui_tex2 = App->tex->Load("gui/uipack_rpg_sheet.png");

		// Loading fonts
		//font = App->font->Load("fonts/PixelCowboy/PixelCowboy.otf", 8);

		// Creates the window
		sets_window = App->gui->CreateBox(&setsBoxes, BOX, 60, 60, { 621, 377, 785, 568 }, gui_tex2, (j1UserInterfaceElement*)sets_window);
		sets_window->visible = true;

		// Sliders
		App->gui->CreateBox(&setsBoxes, BOX, App->gui->lastSlider1X, App->gui->slider1Y, { 388, 455, 28, 42 }, gui_tex2, (j1UserInterfaceElement*)sets_window, App->gui->minimum, App->gui->maximum);
		App->gui->CreateBox(&setsBoxes, BOX, App->gui->lastSlider2X, App->gui->slider2Y, { 388, 455, 28, 42 }, gui_tex2, (j1UserInterfaceElement*)sets_window, App->gui->minimum, App->gui->maximum);

		// Go Back Button
		SDL_Rect idle6 = { 382, 508, 37, 36 };
		SDL_Rect hovered6 = { 343, 508, 37, 36 };
		SDL_Rect clicked6 = { 421, 511, 37, 33 };
		App->gui->CreateButton(&setsButtons, BUTTON, 20, 7, idle6, hovered6, clicked6, gui_tex2, GO_TO_MENU);

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
	App->gui->UpdateButtonsState(&setsButtons);
	App->gui->UpdateSliders(&setsBoxes);
	//App->gui->UpdateWindow(sets_window, &setsButtons, &setsLabels, &setsBoxes);

	// Button actions
	for (std::list<j1Button*>::iterator item = setsButtons.begin(); item != setsButtons.end(); ++item) {
		if ((*item)->visible) {
			switch ((*item)->state)
			{
			case IDLE:
				(*item)->situation = (*item)->idle;
				break;

			case HOVERED:

				if (startup_time.Read() > 2000 && times > 1 || times == 1)
					(*item)->situation = (*item)->hovered;
				break;

			case RELEASED:
				if (startup_time.Read() > 2000 && times > 1 || times == 1) {
					(*item)->situation = (*item)->idle;
					if ((*item)->bfunction == GO_TO_MENU) {
						backToMenu = true;
						App->fade->FadeToBlack();
					}
				}
				break;

			case CLICKED:
				if (startup_time.Read() > 2000 && times > 1 || times == 1)
					(*item)->situation = (*item)->clicked;
				break;
			}
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
	for (std::list<j1Button*>::iterator item = setsButtons.begin(); item != setsButtons.end(); ++item)
	{
		/*if ((*item)->parent != nullptr) continue;*/
		(*item)->Draw(App->gui->buttonsScale);
	}
	for (std::list<j1Label*>::iterator item = setsLabels.begin(); item != setsLabels.end(); ++item)
	{
		if ((*item)->parent != nullptr) continue;
		if ((*item)->visible) (*item)->Draw();
	}

	// Blitting settings window
	if (sets_window != nullptr && sets_window->visible == true)
	{
		sets_window->Draw(App->gui->settingsWindowScale);
	}


	// Blitting the buttons, labels and boxes (sliders) of the window
	for (std::list<j1Button*>::iterator item = setsButtons.begin(); item != setsButtons.end(); ++item) {
		if ((*item)->parent == nullptr) continue;

		if ((*item)->parent->visible == false)
			(*item)->visible = false;
		else
			(*item)->Draw(App->gui->buttonsScale);

	}
	for (std::list<j1Label*>::iterator item = setsLabels.begin(); item != setsLabels.end(); ++item) {
		if ((*item)->parent == nullptr) continue;

		if ((*item)->parent->visible == false)
			(*item)->visible = false;
		else {
			if ((*item)->text == "Settings")
				(*item)->Draw();
			else
				(*item)->Draw(App->gui->buttonsScale);
		}
	}
	for (std::list<j1Box*>::iterator item = setsBoxes.begin(); item != setsBoxes.end(); ++item) {
		if ((*item)->parent == nullptr) continue;

		if ((*item)->parent->visible == false)
			(*item)->visible = false;
		else
			(*item)->Draw(App->gui->buttonsScale);
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

	App->map->CleanUp();
	App->tex->CleanUp();

	for (std::list<j1Button*>::iterator item = setsButtons.begin(); item != setsButtons.end(); ++item) {
		(*item)->CleanUp();
		setsButtons.remove(*item);
	}

	for (std::list<j1Label*>::iterator item = setsLabels.begin(); item != setsLabels.end(); ++item) {
		(*item)->CleanUp();
		setsLabels.remove(*item);
	}

	for (std::list<j1Box*>::iterator item = setsBoxes.begin(); item != setsBoxes.end(); ++item) {
		(*item)->CleanUp();
		setsBoxes.remove(*item);
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