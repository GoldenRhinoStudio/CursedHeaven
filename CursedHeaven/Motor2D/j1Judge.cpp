#include "j1App.h"
#include "j1Judge.h"
#include "j1Entity.h"
#include "j1EntityManager.h"
#include "j1Scene1.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1Render.h"


j1Judge::j1Judge(int x, int y, ENTITY_TYPES type) : j1Entity(x, y, ENTITY_TYPES::JUDGE) 
{
	idle.LoadAnimation("idle", "judge", false);
}


j1Judge::~j1Judge()
{
}

bool j1Judge::Start()
{
	sprites = App->tex->Load("textures/character/judge/Judge.png");
	
	collider = App->collisions->AddCollider({ (int)position.x , (int)position.y + 20, 20,15 }, COLLIDER_NONE, App->entity);

	animation = &idle;

	height = 2;
	return true;
}

bool j1Judge::Update(float dt, bool do_logic)
{

	SDL_Rect* r = &animation->GetCurrentFrame(dt);
	Draw(r);

	return true;
}

bool j1Judge::DrawOrder(float dt)
{
	// Drawing the seller
	SDL_Rect* r = &animation->GetCurrentFrame(dt);
	Draw(r,true);

	return true;
}
