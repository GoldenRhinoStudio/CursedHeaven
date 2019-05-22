#include "j1SceneSettings.h"
#include "j1SceneKeyConfig.h"
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
#include "j1TransitionManager.h"

#include "Brofiler/Brofiler.h"

j1SceneKeyConfig::j1SceneKeyConfig()
{}

j1SceneKeyConfig::~j1SceneKeyConfig()
{}

bool j1SceneKeyConfig::Awake(pugi::xml_node &) {
	LOG("Loading Key Config");

	bool ret = true;

	if (App->menu->active) active = false;

	if (!active) LOG("Key Config Scene not active.");

	return ret;
}

bool j1SceneKeyConfig::Start() {
	return true;
}

bool j1SceneKeyConfig::Update(float dt) {
	return true;
}

bool j1SceneKeyConfig::PostUpdate() {
	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		continueGame = false;

	return continueGame;
}

bool j1SceneKeyConfig::CleanUp() {

	LOG("Freeing Key Scene Textures");

	App->tex->UnLoad(key_tex);
	App->map->CleanUp();
	App->tex->CleanUp();

	for (std::list<j1Button*>::iterator item = key_buttons.begin(); item != key_buttons.end(); ++item) {
		(*item)->CleanUp();
		key_buttons.remove(*item);
	}

	for (std::list<j1Label*>::iterator item = key_labels.begin(); item != key_labels.end(); ++item) {
		(*item)->CleanUp();
		key_labels.remove(*item);
	}

	for (std::list<j1Box*>::iterator item = key_boxes.begin(); item != key_boxes.end(); ++item) {
		(*item)->CleanUp();
		key_boxes.remove(*item);
	}

	return true;
}