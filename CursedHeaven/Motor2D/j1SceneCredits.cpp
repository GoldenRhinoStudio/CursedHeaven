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
		LOG("Scene1 not active.");

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
		App->map->Load("menu.tmx");

		// The audio is played
		App->audio->PlayMusic("audio/music/credits_music.ogg", 1.0f);

		// Loading textures
		gui_tex = App->tex->Load("gui/atlas.png");
		gui_tex2 = App->tex->Load("gui/uipack_rpg_sheet.png");
		license = App->tex->Load("gui/credits.png");

		// Loading fonts
		font = App->font->Load("fonts/PixelCowboy/PixelCowboy.otf", 8);

		SDL_Rect idle = { 260, 470, 190, 49 };
		SDL_Rect hovered = { 260, 519, 190, 49 };
		SDL_Rect clicked = { 260, 568, 190, 49 };

		App->gui->CreateButton(&creditsButtons, BUTTON, 79, 160, idle, hovered, clicked, gui_tex, LINK);

		SDL_Rect idle2 = { 28, 201, 49, 49 };
		SDL_Rect hovered2 = { 77, 201, 49, 49 };
		SDL_Rect clicked2 = { 126, 201, 49, 49 };

		App->gui->CreateButton(&creditsButtons, BUTTON, 229, 3, idle2, hovered2, clicked2, gui_tex, CLOSE_GAME);

		SDL_Rect idle6 = { 382, 508, 37, 36 };
		SDL_Rect hovered6 = { 343, 508, 37, 36 };
		SDL_Rect clicked6 = { 421, 511, 37, 33 };
		App->gui->CreateButton(&creditsButtons, BUTTON, 3, 3, idle6, hovered6, clicked6, gui_tex2, GO_TO_MENU);

		App->gui->CreateLabel(&creditsLabels, LABEL, 90, 165, font, "Webpage", App->gui->beige);

		// Github
		// { 650, 117 , 45, 49 }, { 699, 117 , 45, 49 }, { 748, 113 , 45, 53 }
		SDL_Rect idlegh = { 631, 145 , 36, 40 };
		SDL_Rect hoveredgh = { 631, 221 , 36, 40 };
		SDL_Rect clickedgh = { 631, 185 , 36, 37 };
		App->gui->CreateButton(&creditsButtons, BUTTON, 100, 80, idlegh, hoveredgh, clickedgh, gui_tex2, OPEN_CREDITS);
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

	App->gui->UpdateButtonsState(&creditsButtons);

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
				else if ((*item)->bfunction == CLOSE_GAME) {
					continueGame = false;
				}
				else if ((*item)->bfunction == LINK && App->fade->IsFading() == 0) {
					ShellExecuteA(NULL, "open", "https://goo.gl/SUk3ra", NULL, NULL, SW_SHOWNORMAL);
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

	App->render->Blit(license, 42, 37, NULL, SDL_FLIP_NONE, 1.0f, 0.25);

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
	App->tex->UnLoad(gui_tex);

	App->map->CleanUp();
	App->tex->CleanUp();
	
	for (std::list<j1Button*>::iterator item = creditsButtons.begin(); item != creditsButtons.end(); ++item) {
		(*item)->CleanUp();
		creditsButtons.remove(*item);
	}

	for (std::list<j1Label*>::iterator item = creditsLabels.begin(); item != creditsLabels.end(); ++item) {
		creditsLabels.remove(*item);
	}

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