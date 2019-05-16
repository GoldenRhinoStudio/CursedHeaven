#include "j1Fire.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1FadeToBlack.h"
#include "j1Player.h"
#include "j1BlackMage.h"
#include "j1Map.h"
#include "j1Scene1.h"
#include "j1Audio.h"

#include "Brofiler/Brofiler.h"

j1Fire::j1Fire(int x, int y, ENTITY_TYPES type) : j1Entity(x, y, ENTITY_TYPES::SLIME)
{
	animation = NULL;

	idle.LoadAnimation("idle", "fire", false);	

	// Setting fire position
	initialPosition.x = position.x = x;
	initialPosition.y = position.y = y;
}

j1Fire::~j1Fire() {}

bool j1Fire::Start()
{
	LOG("Loading fire texture");
	sprites = App->tex->Load("textures/enemies/FireEnemy.png");
	debug_tex = App->tex->Load("maps/path2.png");

	LoadProperties();

	animation = &idle;

	collider = App->collisions->AddCollider({ (int)position.x - margin.x, (int)position.y - margin.y, colliderSize.x, colliderSize.y }, COLLIDER_ENEMY, App->entity);

	return true;
}

bool j1Fire::Update(float dt, bool do_logic)
{
	BROFILER_CATEGORY("SlimeUpdate", Profiler::Color::LightSeaGreen)

	if (!dead) {
		collider->SetPos(position.x + margin.x, position.y + margin.y);
		if (!App->entity->currentPlayer->attacking) receivedBasicDamage = false;
		if (!App->entity->currentPlayer->active_Q) receivedAbilityDamage = false;

		iPoint origin = { App->map->WorldToMap((int)position.x + colliderSize.x / 2, (int)position.y + colliderSize.y) };
		iPoint destination = { App->map->WorldToMap((int)App->entity->currentPlayer->position.x + App->entity->currentPlayer->playerSize.x + 1, (int)App->entity->currentPlayer->collider->rect.y + App->entity->currentPlayer->collider->rect.h) };
		//fix destination
		int distance = (int)sqrt(pow(destination.x - origin.x, 2) + pow(destination.y - origin.y, 2));

		if (distance <= DETECTION_RANGE && App->entity->currentPlayer->collider->type == COLLIDER_PLAYER)
		{

			if (App->entity->currentPlayer->dead == false)
			{
				if (do_logic) {
					/*if(path != nullptr)
					path->clear();*/

					if (App->path->CreatePath(origin, destination) > 0) {
						path = App->path->GetLastPath();
						target_found = true;
						node = 0;
					}
					else {
						target_found = false;
					}
				}
				if (target_found && path != nullptr) {
					if (distance <= ATTACK_RANGE_FIRE) {
						App->audio->PlayFx(App->audio->slime_attack);
					}
					Move(path, dt);
				}//fix attack
			}
		}
		else {
			/*	if (path != nullptr)
			path->clear();*/
			target_found = false;
		}

		if (App->entity->currentPlayer->position == App->entity->currentPlayer->initialPosition)
		{
			animation = &idle;
			position = initialPosition;
		}

	}

	App->map->EntityMovement(this);



	return true;
}

bool j1Fire::DrawOrder(float dt) {

	// Drawing the fire
	SDL_Rect* r = &animation->GetCurrentFrame(dt);

	if (position.x - App->entity->currentPlayer->position.x >= 0)
		Draw(r, animation->flip, -10, -10);
	else
		Draw(r, animation->flip, -10, -10);
	return true;
}

bool j1Fire::CleanUp()
{
	LOG("Unloading slime");
	App->tex->UnLoad(sprites);
	if (collider != nullptr)
		collider->to_delete = true;

	/*if (path != nullptr) {
	path->clear();
	RELEASE(path);
	target_found = false;
	}*/

	return true;
}

bool j1Fire::PostUpdate() {

	return true;
}

void j1Fire::OnCollision(Collider * col_1, Collider * col_2)
{
	if (col_2->type == COLLIDER_ATTACK || col_2->type == COLLIDER_ABILITY) {

		if (!receivedBasicDamage && col_2->type == COLLIDER_ATTACK) {
			if (App->entity->player_type == MAGE) col_2->to_delete = true;
			lifePoints -= App->entity->currentPlayer->basicDamage;
			App->audio->PlayFx(App->audio->slime_damage);
			receivedBasicDamage = true;
		}

		if (!receivedAbilityDamage && col_2->type == COLLIDER_ABILITY) {
			col_2->to_delete = true;
			if (App->entity->mage != nullptr)
				lifePoints -= App->entity->mage->fireDamage;
			App->audio->PlayFx(App->audio->slime_damage);

			receivedAbilityDamage = true;
		}

		if (lifePoints <= 0) {
			App->entity->currentPlayer->score_points += 100;
			App->audio->PlayFx(App->audio->slime_death);
			dead = true;
			collider->to_delete = true;

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

bool j1Fire::Load(pugi::xml_node & data)
{
	return true;
}

bool j1Fire::Save(pugi::xml_node& data) const
{
	pugi::xml_node pos = data.append_child("position");

	pos.append_attribute("x") = position.x;
	pos.append_attribute("y") = position.y;

	return true;
}

void j1Fire::LoadProperties()
{
	pugi::xml_document config_file;
	config_file.load_file("config.xml");
	pugi::xml_node config;
	config = config_file.child("config");
	pugi::xml_node fire;
	fire = config.child("fire");

	// Copying the values of the collider
	margin.x = fire.child("margin").attribute("x").as_int();
	margin.y = fire.child("margin").attribute("y").as_int();
	colliderSize.x =  fire.child("colliderSize").attribute("w").as_int();
	colliderSize.y = fire.child("colliderSize").attribute("h").as_int();

	speed = fire.attribute("speed").as_int();
	lifePoints = fire.attribute("life").as_int();
	App->entity->fire_Damage = fire.child("combat").attribute("damage").as_int();

}

void j1Fire::Move(const std::vector<iPoint>* path, float dt)
{
	LOG("Node: %i", node);
	direction = App->path->CheckDirection(path, &node);

	if (direction == Movement::DOWN_RIGHT)
	{
		position.y += speed * dt;
		position.x += speed * dt;
		animation->flip = false;
	}

	else if (direction == Movement::DOWN_LEFT)
	{
		position.y += speed * dt;
		position.x -= speed * dt;
		animation->flip = true;
	}

	else if (direction == Movement::UP_RIGHT)
	{
		position.y -= speed * dt;
		position.x += speed * dt;
		animation->flip = false;
	}

	else if (direction == Movement::UP_LEFT)
	{
		position.y -= speed * dt;
		position.x -= speed * dt;
		animation->flip = true;
	}

	else if (direction == Movement::DOWN)
	{
		position.y += speed * dt;
		animation->flip = false;
	}

	else if (direction == Movement::UP)
	{
		position.y -= speed * dt;
		animation->flip = false;
	}

	else if (direction == Movement::RIGHT)
	{
		position.x += speed * dt;
		animation->flip = false;
	}

	else if (direction == Movement::LEFT)
	{
		position.x -= speed * dt;
		animation->flip = true;
	}
}