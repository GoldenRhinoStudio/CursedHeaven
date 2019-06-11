#include "j1SceneMenu.h"
#include "j1SceneCredits.h"
#include "j1Scene1.h"
#include "j1Scene2.h"
#include "j1App.h"
#include "p2Log.h"
#include "j1Textures.h"
#include "j1EntityManager.h"
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
#include "j1ChooseCharacter.h"
#include "j1SceneSettings.h"
#include "j1TransitionManager.h"

#include "Brofiler/Brofiler.h"

j1SceneMenu::j1SceneMenu()
{
	name.assign("menu");
}

j1SceneMenu::~j1SceneMenu() {}

bool j1SceneMenu::Awake(pugi::xml_node &)
{
	LOG("Loading Menu");
	bool ret = true;

	if (App->menu->active == false)
	{
		LOG("Unable to load the menu.");
		ret = false;
	}

	/*App->audio->MusicVolume(0);
	App->audio->FxVolume(0);*/
	return ret;
}

bool j1SceneMenu::Start()
{	
	if (active)
	{
		// The map is loaded
		App->map->draw_with_quadtrees = false;
		App->map->Load("menu.tmx");

		// The audio is played
	//	App->audio->PlayMusic("audio/music/song012.ogg", 1.0f);

		// Loading textures
		gui_tex2 = App->tex->Load("gui/uipack_rpg_sheet.png");
		
		// Loading fonts
		font = App->font->Load("fonts/Pixeled.ttf", 5);

		// We will use it to check if there is a save file
		pugi::xml_document save_game;
		pugi::xml_parse_result result = save_game.load_file("save_game.xml");

		SDL_Rect idle = { 631, 12, 151, 38 };
		SDL_Rect hovered = {963, 12, 151, 38 };
		SDL_Rect clicked = { 797, 12, 151, 38 };
		App->gui->CreateButton(&menuButtons, BUTTON, 130, 50 + 70, idle, hovered, clicked, gui_tex2, PLAY_GAME);
		App->gui->CreateButton(&menuButtons, BUTTON, 130, 125 + 70, idle, hovered, clicked, gui_tex2, OPEN_CREDITS);
		App->gui->CreateButton(&menuButtons, BUTTON, 130, 150 + 70, idle, hovered, clicked, gui_tex2, CLOSE_GAME);

		SDL_Rect idle4 = { 631, 50, 150, 37 };

		if (result == NULL)
			App->gui->CreateButton(&menuButtons, BUTTON, 130, 75 + 70, idle4, idle4, idle4, gui_tex2, NO_FUNCTION);
		else
			App->gui->CreateButton(&menuButtons, BUTTON, 130, 75 + 70, idle, hovered, clicked, gui_tex2, LOAD_GAME);

		App->gui->CreateButton(&menuButtons, BUTTON, 130, 100 + 70, idle, hovered, clicked, gui_tex2, SETTINGS);

		App->gui->CreateLabel(&menuLabels, LABEL, 152, 120, font, "Start", App->gui->beige);
		App->gui->CreateLabel(&menuLabels, LABEL, 147, 145, font, "Continue", App->gui->beige);
		App->gui->CreateLabel(&menuLabels, LABEL, 147, 170, font, "Settings", App->gui->beige);
		App->gui->CreateLabel(&menuLabels, LABEL, 149, 195, font, "Credits", App->gui->beige);
		App->gui->CreateLabel(&menuLabels, LABEL, 156, 220, font, "Exit", App->gui->beige);

		player_created = false;
		
		startup_time.Start();
		times++;
	}
	return true;
}

bool j1SceneMenu::PreUpdate()
{
	BROFILER_CATEGORY("MenuPreUpdate", Profiler::Color::Orange)
	return true;
}

bool j1SceneMenu::Update(float dt)
{
	BROFILER_CATEGORY("MenuUpdate", Profiler::Color::LightSeaGreen)

	// ---------------------------------------------------------------------------------------------------------------------
	// USER INTERFACE MANAGEMENT
	// ---------------------------------------------------------------------------------------------------------------------	

	// Updating the state of the UI
	App->gui->UpdateButtonsState(&menuButtons, App->gui->buttonsScale);
	App->gui->UpdateSliders(&menuBoxes);
	App->gui->UpdateWindow(settings_window, &menuButtons, &menuLabels, &menuBoxes);

	// Button actions
	for (std::list<j1Button*>::iterator item = menuButtons.begin(); item != menuButtons.end(); ++item) {
		if ((*item)->visible) {
			switch ((*item)->state)
			{
			case IDLE:
				(*item)->situation = (*item)->idle;
				break;

			case HOVERED:

				if (startup_time.Read() > 1900 && times > 1 || times == 1)
					(*item)->situation = (*item)->hovered;
				break;

			case RELEASED:
				if (startup_time.Read() > 1900 && times > 1 || times == 1) {
					(*item)->situation = (*item)->idle;
					if ((*item)->bfunction == PLAY_GAME) {
						LOG("Choose Character Scene Loading");
						App->transitions->FadingToColor(MENU, CHOOSE);
					}
					else if ((*item)->bfunction == LOAD_GAME) {
						App->LoadSpecificModule(App->transitions);

						if (App->transitions->scene1active) 
							App->transitions->SquaresAppearing(MENU, SCENE1, 3);
						if (App->transitions->scene2active)
							App->transitions->SquaresAppearing(MENU, SCENE2, 3);
					}
					else if ((*item)->bfunction == CLOSE_GAME) {
						continueGame = false;
					}
					else if ((*item)->bfunction == SETTINGS) {
						App->transitions->Wiping(MENU, SCENE_SETTINGS);
					}
					else if ((*item)->bfunction == OPEN_CREDITS) {
						App->transitions->Wiping(MENU, CREDITS);
					}
				}
				break;

			case CLICKED:

				if (startup_time.Read() > 1900 && times > 1 || times == 1)
					(*item)->situation = (*item)->clicked;
				break;
			}
		}
	}

	// ---------------------------------------------------------------------------------------------------------------------
	// DRAWING EVERYTHING ON THE SCREEN
	// ---------------------------------------------------------------------------------------------------------------------	

	// Blitting background and animations
	App->map->Draw();

	// Blitting the logo
	SDL_Rect logo = { 854, 92, 801, 166 };
	App->render->Blit(gui_tex2, 30, 30, &logo, SDL_FLIP_NONE, true, App->gui->logoScale);

	// Blitting the buttons and labels of the menu

	for (std::list<j1Button*>::iterator item = menuButtons.begin(); item != menuButtons.end(); ++item)
	{
		if ((*item)->parent != nullptr) continue;
		(*item)->Draw(App->gui->buttonsScale);
	}
	for (std::list<j1Label*>::iterator item = menuLabels.begin(); item != menuLabels.end(); ++item)
	{
		if ((*item)->parent != nullptr) continue;
		if ((*item)->visible) (*item)->Draw();
	}
	
	//Blitting the debug
	if (App->gui->debug) {
		App->render->DrawQuad({ Uint8(3 / 0.25), Uint8(3 / 0.25), 
			Uint8(49 / App->gui->buttonsScale), Uint8(49 / App->gui->buttonsScale) }, 255, 0, 0, 255, false, false);
		App->render->DrawQuad({ Uint8(228 / 0.25) * 6 + 47, Uint8(3 / 0.25),
			Uint8(49 / App->gui->buttonsScale), Uint8(49 / App->gui->buttonsScale) }, 255, 0, 0, 255, false, false);
		App->render->DrawQuad({ 320, 440, 380, Uint8(49 / App->gui->buttonsScale) }, 255, 0, 0, 255, false, false);
		App->render->DrawQuad({ 320, 540, 380, Uint8(49 / App->gui->buttonsScale) }, 255, 0, 0, 255, false, false);
		App->render->DrawQuad({ 320, 640, 380, Uint8(49 / App->gui->buttonsScale) }, 255, 0, 0, 255, false, false);
	}

	return true;
}

bool j1SceneMenu::PostUpdate()
{
	BROFILER_CATEGORY("MenuPostUpdate", Profiler::Color::Yellow)

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		continueGame = false;

	return continueGame;
}

bool j1SceneMenu::CleanUp()
{
	LOG("Freeing all textures");
	App->tex->UnLoad(gui_tex2);
	
	App->map->CleanUp();
	App->tex->CleanUp();

	for (std::list<j1Button*>::iterator item = menuButtons.begin(); item != menuButtons.end(); ++item) {
		(*item)->CleanUp();
		menuButtons.remove(*item);
	}

	for (std::list<j1Label*>::iterator item = menuLabels.begin(); item != menuLabels.end(); ++item) {
		(*item)->CleanUp();
		menuLabels.remove(*item);
	}

	for (std::list<j1Box*>::iterator item = menuBoxes.begin(); item != menuBoxes.end(); ++item) {
		(*item)->CleanUp();
		menuBoxes.remove(*item);
	}

	delete settings_window;
	if(settings_window != nullptr) settings_window = nullptr;

	return true;
}

bool j1SceneMenu::Load(pugi::xml_node& node)
{
	pugi::xml_node activated = node.child("activated");

	bool scene_activated = activated.attribute("true").as_bool();

	return true;
}