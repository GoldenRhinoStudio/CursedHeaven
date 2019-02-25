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
	name.create("credits");
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

		SDL_Rect idle3 = { 312, 292, 49, 49 };
		SDL_Rect hovered3 = { 361, 292, 49, 49 };
		SDL_Rect clicked3 = { 310, 400, 49, 49 };
		App->gui->CreateButton(&creditsButtons, BUTTON, 3, 3, idle3, hovered3, clicked3, gui_tex, GO_TO_MENU);

		App->gui->CreateLabel(&creditsLabels, LABEL, 90, 165, font, "Webpage", App->gui->beige);
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
	for (p2List_item<j1Button*>* item = creditsButtons.start; item != nullptr; item = item->next) {
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
				App->fade->FadeToBlack();
			}
			else if (item->data->bfunction == CLOSE_GAME) {
				continueGame = false;
			}
			else if (item->data->bfunction == LINK) {
				ShellExecuteA(NULL, "open", "https://goo.gl/SUk3ra", NULL, NULL, SW_SHOWNORMAL);
			}
			break;

		case CLICKED:
			item->data->situation = item->data->clicked;
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
	for (p2List_item<j1Button*>* item = creditsButtons.start; item != nullptr; item = item->next) {
		item->data->Draw(App->gui->buttonsScale);
	}

	// Blitting the labels
	for (p2List_item<j1Label*>* item = creditsLabels.start; item != nullptr; item = item->next) {
		item->data->Draw();
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
	
	for (p2List_item<j1Button*>* item = creditsButtons.start; item != nullptr; item = item->next) {
		item->data->CleanUp();
		creditsButtons.del(item);
	}

	for (p2List_item<j1Label*>* item = creditsLabels.start; item != nullptr; item = item->next) {
		creditsLabels.del(item);
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