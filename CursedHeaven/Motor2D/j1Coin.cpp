#include "j1Coin.h"
#include "j1Entity.h"
#include "j1App.h"
#include "j1Player.h"
#include "j1Textures.h"
#include "p2Defs.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Gui.h"
#include "j1Fonts.h"
#include "j1Label.h"

j1Coin::j1Coin(int x, int y, ENTITY_TYPES type)  : j1Entity(x, y, ENTITY_TYPES::COIN)
{
	animation = NULL;

	idle.LoadEnemyAnimations("idle", "coin");

	initialPosition.x = position.x = x;
	initialPosition.y = position.y = y;
	collider = nullptr;
	sprites = nullptr;
}

j1Coin::~j1Coin() {}

bool j1Coin::Start()
{
	collider = App->collisions->AddCollider({ (int)position.x, (int)position.y, 16, 16 }, COLLIDER_COIN, App->entity);
	sprites = App->tex->Load("textures/coin.png");
	if (!sprites) {
		LOG("Error loading coin textures.");
		return false;
	}
	coin_fx = App->audio->LoadFx("audio/fx/coin.wav");
	animation = &idle;

	return true;
}

bool j1Coin::Update(float dt, bool do_logic)
{
	SDL_Rect r = animation->GetCurrentFrame(dt);
		
	Draw(r, false, 0, 0);

	return true;
}

bool j1Coin::CleanUp()
{
	App->tex->UnLoad(sprites);
	if(collider != nullptr)
		collider->to_delete = true;

	return true;
}

void j1Coin::OnCollision(Collider * c1, Collider * c2)
{
	if (c2->type == COLLIDER_PLAYER)
	{
		App->audio->PlayFx(coin_fx);
		App->entity->player->points += 1;
		collider->to_delete = true;
		int num = App->entity->entities.find(this);
		RELEASE(App->entity->entities.At(num)->data);
		App->entity->entities.del(App->entity->entities.At(num));
	}
}

bool j1Coin::Load(pugi::xml_node & data)
{
	return true;
}

bool j1Coin::Save(pugi::xml_node & data) const
{
	pugi::xml_node pos = data.append_child("position");

	pos.append_attribute("x") = position.x;
	pos.append_attribute("y") = position.y;

	return true;
}
