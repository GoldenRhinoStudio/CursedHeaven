#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "j1Button.h"
#include "j1Label.h"
#include "j1Box.h"
#include "j1Audio.h"
#include "j1Window.h"
#include "j1SceneMenu.h"
#include "j1SceneCredits.h"
#include "j1Scene1.h"

#include "Brofiler/Brofiler.h"

j1Gui::j1Gui() : j1Module()
{
	name.assign("gui");
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& config)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = config.child("atlas").attribute("file").as_string("");
	buttonsScale = config.child("scale").attribute("buttonsScale").as_float();
	settingsWindowScale = config.child("scale").attribute("boxScale").as_float();
	logoScale = config.child("scale").attribute("logoScale").as_float();

	// Copying box spawn position
	settingsPosition.x = config.child("positions").attribute("settingsPositionX").as_int();
	settingsPosition.y = config.child("positions").attribute("settingsPositionY").as_int();

	slider1Y = 42;
	slider2Y = 82;
	lastSlider1X = 83;
	lastSlider2X = 83;

	minimum = config.child("sliderLimits").attribute("minimum").as_uint();
	maximum = config.child("sliderLimits").attribute("maximum").as_uint();

	// Copying color values
	pugi::xml_node colors = config.child("colors");

	Uint8 a = colors.attribute("a").as_uint();
	beige = { Uint8(colors.child("beige").attribute("r").as_uint()), Uint8(colors.child("beige").attribute("g").as_uint()), Uint8(colors.child("beige").attribute("b").as_uint()), a};
	brown = { Uint8(colors.child("brown").attribute("r").as_uint()), Uint8(colors.child("brown").attribute("g").as_uint()), Uint8(colors.child("brown").attribute("b").as_uint()), a};
	grey = { Uint8(colors.child("grey").attribute("r").as_uint()), Uint8(colors.child("grey").attribute("g").as_uint()), Uint8(colors.child("grey").attribute("b").as_uint()), a};

	return ret;
}	

// Called before the first frame
bool j1Gui::Start()
{
	atlas = App->tex->Load(atlas_file_name.data());

	// Audios are loaded
	LOG("Loading player audios");
	if (!loadedAudios) {
		clickSound = App->audio->LoadFx("audio/fx/click.wav");
		hoverSound = App->audio->LoadFx("audio/fx/hover.wav");
		loadedAudios = true;
	}

	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	BROFILER_CATEGORY("GuiPreUpdate", Profiler::Color::Orange)

	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
		debug = !debug;

	return true;
}

// Called after all Updates
bool j1Gui::PostUpdate()
{
	BROFILER_CATEGORY("GuiPostUpdate", Profiler::Color::Yellow)

	// Blitting settings windows
	if (App->scene1->settings_window != nullptr && App->scene1->settings_window->visible == true)
		App->scene1->settings_window->Draw(App->gui->settingsWindowScale);

	//-------------------------


	for (std::list<j1Button*>::iterator item = App->scene1->scene1Buttons.begin(); item != App->scene1->scene1Buttons.end(); ++item)
	{
		if ((*item)->parent == nullptr) continue;

		if ((*item)->parent->visible == false)
			(*item)->visible = false;
		else
			(*item)->Draw(App->gui->buttonsScale);
	}

	for (std::list<j1Label*>::iterator item = App->scene1->scene1Labels.begin(); item != App->scene1->scene1Labels.end(); ++item) 
	{
		if ((*item)->parent == nullptr) continue;

		if ((*item)->parent->visible == false)
			(*item)->visible = false;

		else 
		{
			if ((*item)->text != "Settings" && (*item)->text != "Save"  && (*item)->text != "Quit")
				(*item)->Draw(App->gui->buttonsScale);
			else
				(*item)->Draw();
		}
	}

	for (std::list<j1Box*>::iterator item = App->scene1->scene1Boxes.begin(); item != App->scene1->scene1Boxes.end(); ++item) 
	{
		if ((*item)->parent == nullptr) continue;

		if ((*item)->parent->visible == false)
			(*item)->visible = false;
		else
			(*item)->Draw(App->gui->buttonsScale);
	}

	return true;
}

// Factiry methods
j1Button* j1Gui::CreateButton(std::list<j1Button*>* buttons, UIELEMENT_TYPES type, int x, int y, SDL_Rect idle, SDL_Rect hovered, SDL_Rect clicked, SDL_Texture* text, ButtonFunction function, j1UserInterfaceElement* parent) {
	j1Button* ret = nullptr;

	ret = new j1Button(type, x, y, idle, hovered, clicked, text, function, parent);
	if (ret != nullptr) buttons->push_back(ret);

	return ret;
}

j1Label* j1Gui::CreateLabel(std::list<j1Label*>* labels, UIELEMENT_TYPES type, int x, int y, _TTF_Font* font, const char* text, SDL_Color color, j1UserInterfaceElement* parent) {
	j1Label* ret = nullptr;

	ret = new j1Label(type, x, y, font, text, color, parent);
	if (ret != nullptr) labels->push_back(ret);

	return ret;
}

j1Box* j1Gui::CreateBox(std::list<j1Box*>* boxes, UIELEMENT_TYPES type, int x, int y, SDL_Rect section, SDL_Texture* text, j1UserInterfaceElement* parent, uint minimum, uint maximum) {
	j1Box* ret = nullptr;

	ret = new j1Box(type, x, y, section, text, parent, minimum, maximum);
	if (ret != nullptr) boxes->push_back(ret);

	return ret;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI"); 

	return true;
}

// const getter for atlas
const SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

void j1Gui::UpdateButtonsState(std::list<j1Button*>* buttons, float scale) {
	int x, y; App->input->GetMousePosition(x, y);
	
	for (std::list<j1Button*>::iterator item = buttons->begin(); item != buttons->end(); ++item)
	{

		if ((*item)->visible == false || (*item)->bfunction == NO_FUNCTION) continue;

		if (x - (App->render->camera.x / (int)(App->win->GetScale())) <= (*item)->position.x + (*item)->situation.w * scale
			&& x - (App->render->camera.x / (int)(App->win->GetScale())) >= (*item)->position.x
			&& y - (App->render->camera.y / (int)(App->win->GetScale())) <= (*item)->position.y + (*item)->situation.h * scale
			&& y - (App->render->camera.y / (int)(App->win->GetScale())) >= (*item)->position.y) {

			if(App->credits->active == false && App->menu->settings_window != nullptr && App->menu->settings_window->visible
				&& (*item)->bfunction != CLOSE_SETTINGS) continue;

			(*item)->state = STATE::HOVERED;
			if (!(*item)->hoverPlayed) {
				App->audio->PlayFx(hoverSound);
				(*item)->hoverPlayed = true;
			}

			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
				(*item)->state = STATE::CLICKED;
				if (!(*item)->clickPlayed) {
					App->audio->PlayFx(clickSound);
					(*item)->clickPlayed = true;
				}
			}
			if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
				(*item)->state = STATE::RELEASED;
				(*item)->clickPlayed = false;
			}
		}
		else {
			(*item)->state = STATE::IDLE;
			(*item)->hoverPlayed = false;
		}
	}
}

void j1Gui::UpdateWindow(j1Box* window, std::list<j1Button*>* buttons, std::list<j1Label*>* labels, std::list<j1Box*>* boxes) {
	int x, y; App->input->GetMousePosition(x, y);

	// Checking if it is being clicked
	if (window != nullptr && window->visible == true 
		&& x - (App->render->camera.x / (int)(App->win->GetScale())) <= window->position.x + window->section.w * App->gui->settingsWindowScale
		&& x - (App->render->camera.x / (int)(App->win->GetScale())) >= window->position.x
		&& y - (App->render->camera.y / (int)(App->win->GetScale())) <= window->position.y + window->section.h * App->gui->settingsWindowScale
		&& y - (App->render->camera.y / (int)(App->win->GetScale())) >= window->position.y)
	{
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
			window->clicked = true;
		else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
			window->clicked = false;
	}

	// We move the window in case it is clicked
	if (window != nullptr) {

		if (buttons != nullptr) 
		{
			for (std::list<j1Button*>::iterator item = buttons->begin(); item != buttons->end(); ++item)
				if ((*item)->state == CLICKED && (*item)->parent == window) window->clicked = false;	
		}

		if (boxes != nullptr) {
			for (std::list<j1Box*>::iterator item = boxes->begin(); item != boxes->end(); ++item)
				if ((*item)->clicked && (*item)->parent == window) window->clicked = false;
		}

		if (window->clicked) 
		{
			int x, y; App->input->GetMousePosition(x, y);

			if (window->distanceCalculated == false) {
				window->mouseDistance = { x - window->position.x, y - window->position.y };
				window->distanceCalculated = true;
			}

			// Updating the positions of the window and its elements
			window->position = { x - window->mouseDistance.x, y - window->mouseDistance.y };

			if (buttons != nullptr) {
				for (std::list<j1Button*>::iterator item = buttons->begin(); item != buttons->end(); ++item) {
					if ((*item)->parent == window) {
						(*item)->position.x = window->position.x + (*item)->initialPosition.x;
						(*item)->position.y = window->position.y + (*item)->initialPosition.y;
					}
				}
			}

			if (labels != nullptr) {
				for (std::list<j1Label*>::iterator item = labels->begin(); item != labels->end(); ++item) {
					if ((*item)->parent == window) {
						(*item)->position.x = window->position.x + (*item)->initialPosition.x;
						(*item)->position.y = window->position.y + (*item)->initialPosition.y;
					}
				}
			}

			if (boxes != nullptr) {
				for (std::list<j1Box*>::iterator item = boxes->begin(); item != boxes->end(); ++item) {
					if ((*item)->parent == window) {
						(*item)->position.x = window->position.x + (*item)->initialPosition.x;
						(*item)->position.y = window->position.y + (*item)->initialPosition.y;
						
						(*item)->minimum = (*item)->originalMinimum + window->position.x;
						(*item)->maximum = (*item)->originalMaximum + window->position.x;
					}
				}
			}
		}
		else
			window->distanceCalculated = false;
	}
}

void j1Gui::UpdateSliders(std::list<j1Box*>* sliders) {
	int x, y; App->input->GetMousePosition(x, y);

	// Checking if they are being dragged
	
	for (std::list<j1Box*>::iterator item = sliders->begin(); item != sliders->end(); ++item)
	{
		if ((*item)->parent != nullptr && (*item)->visible == true)
		{
			if (x - (App->render->camera.x / (int)(App->win->GetScale())) <= (*item)->position.x + (*item)->section.w * App->gui->buttonsScale
				&& x - (App->render->camera.x / (int)(App->win->GetScale())) >= (*item)->position.x
				&& y - (App->render->camera.y / (int)(App->win->GetScale())) <= (*item)->position.y + (*item)->section.h * App->gui->buttonsScale 
				&& y - (App->render->camera.y / (int)(App->win->GetScale())) >= (*item)->position.y)
			{
				if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
					(*item)->clicked = true;
				else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
					(*item)->clicked = false;
					(*item)->initialPosition.x = (*item)->position.x - (*item)->parent->position.x;
				}
			}
			else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
				(*item)->clicked = false;
				(*item)->initialPosition.x = (*item)->position.x - (*item)->parent->position.x;
			}
		}
	}

	// Moving sliders
	for (std::list<j1Box*>::iterator item = sliders->begin(); item != sliders->end(); ++item) {
		if ((*item)->clicked && (*item)->parent != nullptr) {
			int x, y; App->input->GetMousePosition(x, y);

			uint lastPos = (*item)->position.x;

			if ((*item)->distanceCalculated == false) {
				(*item)->mouseDistance.x = x - (*item)->position.x;
				(*item)->distanceCalculated = true;
			}

			(*item)->position.x = x - (*item)->mouseDistance.x;

			// The default value for the margins is 0, meaning they have no minimum or maximum
			if ((*item)->minimum != 0 && (*item)->position.x <= (*item)->minimum)
				(*item)->position.x = (*item)->minimum;
			if ((*item)->maximum != 0 && (*item)->position.x >= (*item)->maximum)
				(*item)->position.x = (*item)->maximum;

			// After that we change the volume
			if ((*item)->position.y < (*item)->parent->position.y + 50) {
				if((*item)->position.x > lastPos)
					App->audio->FxVolume(App->audio->GetFxVolume() + ((*item)->position.x - lastPos) * 2);
				else
					App->audio->FxVolume(App->audio->GetFxVolume() - (lastPos - (*item)->position.x) * 2);

				lastSlider1X = (*item)->position.x - (*item)->parent->position.x;
			}
			else {
				if ((*item)->position.x > lastPos)
					App->audio->MusicVolume(App->audio->GetMusicVolume() + ((*item)->position.x - lastPos) * 2);
				else
					App->audio->MusicVolume(App->audio->GetMusicVolume() - (lastPos - (*item)->position.x) * 2);

				lastSlider2X = (*item)->position.x - (*item)->parent->position.x;
			}
		}
	}
}

// class Gui ---------------------------------------------------