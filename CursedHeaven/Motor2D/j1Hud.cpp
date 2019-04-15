#include "j1Hud.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1UserInterfaceElement.h"
#include "j1Label.h"
#include "j1Scene1.h"
#include "j1Fonts.h"
#include "j1Render.h"
#include "j1Player.h"
#include "j1Input.h"
#include "j1Window.h"

j1Hud::j1Hud() 
{
	
}

j1Hud::~j1Hud() {}

bool j1Hud::Start()
{
	hud_text = App->tex->Load("gui/hud.png");
	
	return true;
}

bool j1Hud::Update(float dt)
{
	App->gui->UpdateButtonsState(&hud_buttons, App->gui->buttonsScale);

	for (std::list<j1Button*>::iterator item = hud_buttons.begin(); item != hud_buttons.end(); ++item) {
		if ((*item)->visible) {
			switch ((*item)->state) {
			case IDLE:
				(*item)->situation = (*item)->idle;
				break;

			case HOVERED:
				(*item)->situation = (*item)->hovered;
				break;

			case RELEASED:
				(*item)->situation = (*item)->idle;
				break;

			case CLICKED:
				(*item)->situation = (*item)->clicked;
				break;
			}
		}
	}

	SDL_Rect dk_profile = { 0,0,42,42 };
	SDL_Rect bm_profile = { 42,0,43,42 };

	if (App->entity->player_type == MAGE)
		black_mage = true;
	else if (App->entity->player_type == KNIGHT)
		dragoon_knight = true;

	if (black_mage) App->render->Blit(hud_text, 5, 5, &bm_profile, SDL_FLIP_NONE, 1.0f, 1.0f, 0.0, pivot, pivot, false);
	else if (dragoon_knight) App->render->Blit(hud_text, 5, 5, &dk_profile, SDL_FLIP_NONE, 1.0f, 1.0f, 0.0, pivot, pivot, false);

	for (std::list<j1Button*>::iterator item = hud_buttons.begin(); item != hud_buttons.end(); ++item) {
		(*item)->Draw(App->gui->buttonsScale);
	}

	return true;
}

bool j1Hud::CleanUp()
{
	App->tex->UnLoad(hud_text);

	for (std::list<j1Button*>::iterator item = hud_buttons.begin(); item != hud_buttons.end(); ++item) {
		(*item)->CleanUp();
		hud_buttons.remove(*item);
	}

	return true;
}

bool j1Hud::Load(pugi::xml_node & data)
{
	return true;
}

bool j1Hud::Save(pugi::xml_node & data) const
{
	return true;
}
