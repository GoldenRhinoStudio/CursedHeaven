#include "j1App.h"
#include "j1Seller.h"
#include "j1Entity.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1EntityManager.h"
#include <time.h>

j1Seller::j1Seller(int x, int y, ENTITY_TYPES type) : j1Entity(x, y, ENTITY_TYPES::NPC) {

	animation = NULL; 
	idle.LoadAnimation("idle", "seller", false);
}

j1Seller::~j1Seller() {}

bool j1Seller::Start()
{
	LOG("Loading seller texture");
	sprites = App->tex->Load("textures/character/Rogue/Rogue.png");

	animation = &idle;
	height = 2;

	collider = App->collisions->AddCollider({ (int)position.x, (int)position.y, 30, 30 }, COLLIDER_PLAYER, App->entity);
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
	LOG("Loading seller texture");
	App->tex->UnLoad(sprites);
	return true;
}