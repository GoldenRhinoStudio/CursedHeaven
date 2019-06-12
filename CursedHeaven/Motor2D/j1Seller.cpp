#include "j1App.h"
#include "j1Seller.h"
#include "j1Entity.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1EntityManager.h"
#include "j1Entity.h"
#include <time.h>

j1Seller::j1Seller(int x, int y, ENTITY_TYPES type) : j1Entity(x, y, ENTITY_TYPES::SELLER) {

	animation = NULL;
	idle.LoadAnimation("idle", "seller", false);
}

j1Seller::~j1Seller() {}

bool j1Seller::Start()
{
	LOG("Loading seller texture");
	sprites = App->tex->Load("textures/character/Rogue/Rogue.png");

	collider = App->collisions->AddCollider({ (int)position.x , (int)position.y+ 20, 20,15}, COLLIDER_NONE, App->entity);

	animation = &idle;
	height = 2;

	return true;
}

bool j1Seller::DrawOrder(float dt)
{
	// Drawing the seller
	SDL_Rect* r = &animation->GetCurrentFrame(dt);
	Draw(r);

	return true;
}


bool j1Seller::Update(float dt, bool do_logic) {

	return true;
}

bool j1Seller::CleanUp()
{
	LOG("Unloading Seller");
	App->tex->UnLoad(sprites);
	App->tex->UnLoad(debug_tex);

	sprites = nullptr;
	debug_tex = nullptr;

	if (collider != nullptr)
		collider->to_delete = true;

	animation = nullptr;


	return true;
}