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

j1SceneCredits::j1SceneCredits()
{
	name.assign("credits");
}

j1SceneCredits::~j1SceneCredits() {}

bool j1SceneCredits::Awake(pugi::xml_node &)
{
	LOG("Loading Credits");
	bool ret = true;

	if (App->menu->active == true)
		active = false;

	if (active == false)
		LOG("Credits not active.");

	if (App->menu->active == false)
	{
		LOG("Unable to load the credits.");
		ret = false;
	}
	
	return ret;
}

bool j1SceneCredits::Start()
{
	if (active)
	{
		// The map is loaded
		App->map->draw_with_quadtrees = false;
		App->map->Load("menu.tmx");

		// The audio is played
		App->audio->PlayMusic("audio/music/credits_music.ogg", 1.0f);

		// Loading textures
		gui_tex2 = App->tex->Load("gui/uipack_rpg_sheet.png");
		license = App->tex->Load("gui/creditsCURSEDHEAVEN.png");

		credits_window = App->gui->CreateBox(&creditsBoxes, BOX, 45, 36, { 621, 377, 785, 568 }, gui_tex2);
		credits_window->visible = true;

		// Go Back 
		SDL_Rect idle6 = { 382, 508, 37, 36 };
		SDL_Rect hovered6 = { 343, 508, 37, 36 };
		SDL_Rect clicked6 = { 421, 508, 37, 36 };
		App->gui->CreateButton(&creditsButtons, BUTTON, 7, 7, idle6, hovered6, clicked6, gui_tex2, GO_TO_MENU);

		// Github
		SDL_Rect idlegh = { 631, 145 , 36, 39 };
		SDL_Rect hoveredgh = { 631, 223 , 36, 39 };
		SDL_Rect clickedgh = { 631, 184 , 36, 39 };
		App->gui->CreateButton(&creditsButtons, BUTTON, 151, 188, idlegh, hoveredgh, clickedgh, gui_tex2, LINK, (j1UserInterfaceElement*)credits_window);

		// Web
		SDL_Rect idleweb = { 667, 145 , 36, 39 };
		SDL_Rect hoveredweb = { 667, 223 , 36, 39 };
		SDL_Rect clickedweb = { 667, 184 , 36, 39 };
		App->gui->CreateButton(&creditsButtons, BUTTON, 171, 188, idleweb, hoveredweb, clickedweb, gui_tex2, LINK2, (j1UserInterfaceElement*)credits_window);

		startup_time.Start();
	}						 

	return true;
}

bool j1SceneCredits::PreUpdate()
{
	BROFILER_CATEGORY("CreditsPreUpdate", Profiler::Color::Orange)
		return true;
}

bool j1SceneCredits::Update(float dt)
{
	BROFILER_CATEGORY("CreditsUpdate", Profiler::Color::LightSeaGreen)
		
	// ---------------------------------------------------------------------------------------------------------------------
	// USER INTERFACE MANAGEMENT
	// ---------------------------------------------------------------------------------------------------------------------	

	App->gui->UpdateButtonsState(&creditsButtons, App->gui->buttonsScale);

	// Button actions
	for (std::list<j1Button*>::iterator item = creditsButtons.begin(); item != creditsButtons.end(); ++item) {
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
			if (startup_time.Read() > 2000) {
				(*item)->situation = (*item)->idle;
				
				if ((*item)->bfunction == GO_TO_MENU) {
					backToMenu = true;
					App->fade->FadeToBlack();
				}
				else if ((*item)->bfunction == LINK && App->fade->IsFading() == 0) {
					ShellExecuteA(NULL, "open", "https://github.com/GoldenRhinoStudio/CursedHeaven", NULL, NULL, SW_SHOWNORMAL);
				}
				else if ((*item)->bfunction == LINK2 && App->fade->IsFading() == 0) {
					ShellExecuteA(NULL, "open", "https://goldenrhinostudio.com/", NULL, NULL, SW_SHOWNORMAL);
				}
			}
			break;

		case CLICKED:
			if (startup_time.Read() > 2000)
			(*item)->situation = (*item)->clicked;
			break;
		}
	}
	
	if (backToMenu && App->fade->IsFading() == 0) {
		ChangeScene();
	}

	// ---------------------------------------------------------------------------------------------------------------------
	// DRAWING EVERYTHING ON THE SCREEN
	// ---------------------------------------------------------------------------------------------------------------------	

	App->map->Draw();

	// Blitting the buttons
	for (std::list<j1Button*>::iterator item = creditsButtons.begin(); item != creditsButtons.end(); ++item) {
		(*item)->Draw(App->gui->buttonsScale);
	}

	// Blitting the labels
	for (std::list<j1Label*>::iterator item = creditsLabels.begin(); item != creditsLabels.end(); ++item) {
		(*item)->Draw();
	}

	if (credits_window != nullptr && credits_window->visible == true)
	{
		credits_window->Draw(App->gui->creditsWindowScale);
	}

	// Blitting buttons of the window
	for (std::list<j1Button*>::iterator item = creditsButtons.begin(); item != creditsButtons.end(); ++item) {
		if ((*item)->parent == nullptr) continue;

		if ((*item)->parent->visible == false)
			(*item)->visible = false;
		else
			(*item)->Draw(App->gui->buttonsScale);

	}

	App->render->Blit(license, 50, 50, NULL, SDL_FLIP_NONE, false, 0.24f);



	return true;
}

bool j1SceneCredits::PostUpdate()
{
	BROFILER_CATEGORY("CreditsPostUpdate", Profiler::Color::Yellow)

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		continueGame = false;

	return continueGame;
}

bool j1SceneCredits::CleanUp()
{
	LOG("Freeing all textures");
	App->tex->UnLoad(gui_tex2);

	App->map->CleanUp();
	App->tex->CleanUp();
	
	for (std::list<j1Button*>::iterator item = creditsButtons.begin(); item != creditsButtons.end(); ++item) {
		(*item)->CleanUp();
		creditsButtons.remove(*item);
	}

	for (std::list<j1Label*>::iterator item = creditsLabels.begin(); item != creditsLabels.end(); ++item) {
		creditsLabels.remove(*item);
	}

	for (std::list<j1Box*>::iterator item = creditsBoxes.begin(); item != creditsBoxes.end(); ++item) {
		(*item)->CleanUp();
		creditsBoxes.remove(*item);
	}

	delete credits_window;
	if (credits_window != nullptr) credits_window = nullptr;

	return true;
}

void j1SceneCredits::ChangeScene()
{
	this->active = false;
	backToMenu = false;
	CleanUp();
	App->menu->active = true;
	App->menu->Start();	
}