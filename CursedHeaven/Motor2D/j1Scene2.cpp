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
#include "j1DragoonKnight.h"
#include "j1BlackMage.h"
#include "j1Tank.h"
#include "j1Rogue.h"
#include "j1Judge.h"
#include "j1SceneMenu.h"
#include "j1Scene1.h"
#include "j1Scene2.h"
#include "j1FadeToBlack.h"
#include "j1Pathfinding.h"
#include "j1Gui.h"
#include "j1SceneMenu.h"
#include "j1Label.h"
#include "j1Button.h"
#include "j1Box.h"
#include "j1ChooseCharacter.h"
#include "j1DialogSystem.h"
#include "j1Particles.h"
#include "j1SceneLose.h"
#include "j1SceneVictory.h"
#include "j1Shop.h"
#include "j1Minimap.h"
#include "j1TransitionManager.h"

#include "Brofiler/Brofiler.h"

j1Scene2::j1Scene2() : j1Module() {
	name.assign("scene2");
}

j1Scene2::~j1Scene2()
{}

bool j1Scene2::Awake(pugi::xml_node& config) {
	LOG("Loading Scene 2");

	bool ret = true;

	if (App->menu->active)
		active = false;

	if (App->scene1->active)
		active = false;

	if (!active)
		LOG("Scene2 not active.");

	return ret;
}

bool j1Scene2::Start() {

	if (active) {
		App->map->draw_with_quadtrees = true;

		if (App->map->Load("skycrown.tmx")) {
			int w, h;
			uchar* data = NULL;
			if (App->map->CreateWalkabilityMap(w, h, &data))
			{
				App->path->SetMap(w, h, data);
			}

			RELEASE_ARRAY(data);
		}

		// AUDIO

		// TEXTURES
		debug_tex = App->tex->Load("maps/path2.png");
		gui_tex = App->tex->Load("gui/uipack_rpg_sheet.png");

	}

	return true;
}

bool j1Scene2::PreUpdate() {
	
	BROFILER_CATEGORY("Level2PreUpdate", Profiler::Color::Orange)


	return true;
}

bool j1Scene2::Update(float dt) {
	
	BROFILER_CATEGORY("Level2Update", Profiler::Color::LightSeaGreen)

	App->map->Draw();
	App->entity->DrawEntityOrder(dt);
	App->render->reOrder();
	App->render->OrderBlit(App->render->OrderToRender);

	return true;
}

bool j1Scene2::PostUpdate() {
	return true;
}

bool j1Scene2::Load(pugi::xml_node& node) {
	return true;
}

bool j1Scene2::Save(pugi::xml_node& node) const {
	pugi::xml_node activated = node.append_child("activated");

	activated.append_attribute("true") = active;

	return true;
}

void j1Scene2::PlaceEntities(int room) {

}

bool j1Scene2::CleanUp() {

	LOG("Freeing scene2");

	App->tex->UnLoad(gui_tex);
	App->tex->UnLoad(debug_tex);
	App->tex->UnLoad(lvl2_tex);
	App->map->CleanUp();
	App->collisions->CleanUp();
	App->tex->CleanUp();
	App->entity->DestroyEntities();
	App->gui->CleanUp();
	App->particles->CleanUp();

	if (App->entity->knight) App->entity->knight->CleanUp();
	if (App->entity->mage) App->entity->mage->CleanUp();


	App->path->CleanUp();
	App->shop->CleanUp();

	return true;
}