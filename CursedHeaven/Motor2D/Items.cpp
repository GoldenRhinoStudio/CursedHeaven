#include "Items.h"
#include "j1Slime.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1Render.h"
#include "j1FadeToBlack.h"
#include "j1Player.h"
#include "j1Map.h"
#include "j1Scene1.h"
#include "j1Audio.h"

#include "Brofiler/Brofiler.h"


Items::Items(int x, int y, ENTITY_TYPES type) : j1Entity(x, y, type) 
{
	animation = NULL;

	switch (type) {
	case(COIN):
		item.LoadAnimation("coin", "items", false);
		break;
	case (LIFE):
		item.LoadAnimation("heart", "items", false);
		break;
	}

	this->type = type;

	// Setting position
	initialPosition.x = position.x = x;
	initialPosition.y = position.y = y;
	Start();
}

Items::~Items(){}

bool Items::Start()
{
	LOG("Loading slime texture");

	sprites = App->tex->Load("textures/Collectibles/Enemy_drops.png");

	LoadProperties();

	animation = &item;

	collider = App->collisions->AddCollider({ (int)position.x, (int)position.y, item.frames->w, item.frames->h }, COLLIDER_DROP, App->entity);

	return true;
}

bool Items::Update(float dt, bool do_logic)
{
	return true;
}

bool Items::DrawOrder(float dt) {

	// Drawing the slime
	SDL_Rect* r = &animation->GetCurrentFrame(dt);

	Draw(r);
	return true;
}

bool Items::CleanUp()
{
	LOG("Unloading slime");
	App->tex->UnLoad(sprites);
	sprites = nullptr;
	if (collider != nullptr)
		collider->to_delete = true;

	/*if (path != nullptr) {
		path->clear();
		RELEASE(path);
		target_found = false;
	}*/

	return true;
}

bool Items::PostUpdate() {

	return true;
}

void Items::OnCollision(Collider * col_1, Collider * col_2)
{
	if (col_2->type == COLLIDER_PLAYER)
	{
		if (type == COIN) {
			collider->to_delete = true;
			App->entity->currentPlayer->coins += value;
			App->audio->PlayFx(App->audio->coin_sound);
			
		}
		else if (type == LIFE) {
			if (App->entity->currentPlayer->lifePoints + value <= App->entity->currentPlayer->totalLifePoints) {
				collider->to_delete = true;
				App->entity->currentPlayer->lifePoints += value;
				App->audio->PlayFx(App->audio->heal_sound);
			}
		}
		if (collider->to_delete) {
			for (std::list<j1Entity*>::iterator item = App->entity->entities.begin(); item != App->entity->entities.end(); ++item) {
				if (item._Ptr->_Myval == this) {
					RELEASE(item._Ptr->_Myval);
					App->entity->entities.remove(item._Ptr->_Myval);
					break;
				}
			}
		}
	}
}

bool Items::Load(pugi::xml_node & data)
{
	return true;
}

bool Items::Save(pugi::xml_node& data) const
{
	pugi::xml_node pos = data.append_child("position");

	pos.append_attribute("x") = position.x;
	pos.append_attribute("y") = position.y;

	return true;
}

void Items::LoadProperties()
{
	pugi::xml_document config_file;
	config_file.load_file("config.xml");
	pugi::xml_node config;
	config = config_file.child("config");
	pugi::xml_node items;
	items = config.child("items");
	pugi::xml_node item;
	item = items.child(ToString(type));

	// Copying the values of the item
	value = item.attribute("value").as_int();
}

const char* Items::ToString(ENTITY_TYPES item)
{
	switch (item)
	{
	case COIN:   return "coin";
	case LIFE:   return "heart";
	default:	 return "coin";
	}
}