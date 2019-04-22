#include "j1Slime.h"
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

#include "Brofiler/Brofiler.h"

j1Slime::j1Slime(int x, int y, ENTITY_TYPES type) : j1Entity(x, y, ENTITY_TYPES::SLIME)
{
	animation = NULL;

	idle_diagonal_up.LoadAnimation("idleD_up", "slime", false);
	idle_diagonal_down.LoadAnimation("idleD_down", "slime", false);
	idle_lateral.LoadAnimation("idleLateral", "slime", false);
	idle_down.LoadAnimation("idleDown", "slime", false);
	idle_up.LoadAnimation("idleUp", "slime", false);

	diagonal_up.LoadAnimation("diagonalUp", "slime", false);
	diagonal_down.LoadAnimation("diagonalDown", "slime", false);
	lateral.LoadAnimation("lateral", "slime", false);
	up.LoadAnimation("up", "slime", false);
	down.LoadAnimation("down", "slime", false);

	// Setting harpy position
	initialPosition.x = position.x = x;
	initialPosition.y = position.y = y;
}

j1Slime::~j1Slime() {}

bool j1Slime::Start()
{
	// Textures are loaded
	LOG("Loading slime textures");
	sprites = App->tex->Load("textures/enemies/Slime.png");
	debug_tex = App->tex->Load("maps/path2.png");

	LoadProperties();

	animation = &idle_down;

	collider = App->collisions->AddCollider({ (int)position.x - margin.x, (int)position.y - margin.y, colliderSize.x, colliderSize.y }, COLLIDER_ENEMY, App->entity);

	return true;
}

bool j1Slime::Update(float dt, bool do_logic)
{
	BROFILER_CATEGORY("SlimeUpdate", Profiler::Color::LightSeaGreen)

	if (!dead) {
		collider->SetPos(position.x, position.y);
		if (!App->entity->currentPlayer->attacking) receivedBasicDamage = false;
		if (!App->entity->currentPlayer->active_Q) receivedAbilityDamage = false; 
		
		iPoint origin = { App->map->WorldToMap((int)position.x + colliderSize.x / 2, (int)position.y + colliderSize.y) };
		iPoint destination = { App->map->WorldToMap((int)App->entity->currentPlayer->position.x + App->entity->currentPlayer->playerSize.x + 1, (int)App->entity->currentPlayer->position.y + App->entity->currentPlayer->playerSize.y) };

		int distance = (int)sqrt(pow(destination.x - origin.x, 2) + pow(destination.y - origin.y, 2));

		if (distance <= DETECTION_RANGE && App->entity->currentPlayer->collider->type == COLLIDER_PLAYER)
		{
			
			if (App->entity->currentPlayer->dead == false)
			{
				if (do_logic) {
					if(path != nullptr)
						path->clear();

					if (App->path->CreatePath(origin, destination) > 0) {
						path = App->path->GetLastPath();
						target_found = true;
					}
					else {
						target_found = false;
					}
				}
				if (target_found && path != nullptr) {
					if (distance <= ATTACK_RANGE) {
						//atack slime
					}
					else {
						Move(path, dt);
					}
				}
			}
		}
		else {
			if (path != nullptr)
				path->clear();
			target_found = false;
		}

		if (App->entity->currentPlayer->position == App->entity->currentPlayer->initialPosition)
		{
			animation = &idle_down;
			position = initialPosition;
		}

	}

	App->map->EntityMovement(this);


	
	return true;
}

bool j1Slime::DrawOrder(float dt) {

	// Drawing the harpy
	SDL_Rect* r = &animation->GetCurrentFrame(dt);

	if (position.x - App->entity->currentPlayer->position.x >= 0)
		Draw(r, animation->flip, -10, -10);
	else
		Draw(r, animation->flip, -10, -10);
	return true;
}

bool j1Slime::CleanUp()
{
	LOG("Unloading harpy");
	App->tex->UnLoad(sprites);
	if (collider != nullptr)
		collider->to_delete = true;

	if (path != nullptr) {
		path->clear();
		RELEASE(path);
		target_found = false;
	}

	return true;
}

bool j1Slime::PostUpdate() {
	return true;
}

void j1Slime::OnCollision(Collider * col_1, Collider * col_2)
{
	if (col_2->type == COLLIDER_ATTACK || col_2->type == COLLIDER_ABILITY) {
		
		if (!receivedBasicDamage && col_2->type == COLLIDER_ATTACK) {
			lifePoints -= App->entity->currentPlayer->basicDamage;
			receivedBasicDamage = true;
		}

		if (!receivedAbilityDamage && col_2->type == COLLIDER_ABILITY) {
			if(App->entity->mage != nullptr)
				lifePoints -= App->entity->mage->fireDamage;

			receivedAbilityDamage = true;
		}

		if (lifePoints <= 0) {
			App->entity->currentPlayer->score_points += 100;
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

bool j1Slime::Load(pugi::xml_node & data)
{
	return true;
}

bool j1Slime::Save(pugi::xml_node& data) const
{
	pugi::xml_node pos = data.append_child("position");

	pos.append_attribute("x") = position.x;
	pos.append_attribute("y") = position.y;

	return true;
}

void j1Slime::LoadProperties()
{
	pugi::xml_document config_file;
	config_file.load_file("config.xml");
	pugi::xml_node config;
	config = config_file.child("config");
	pugi::xml_node slime;
	slime = config.child("slime");

	speed = slime.attribute("speed").as_int();
	lifePoints = slime.attribute("life").as_int();

	// Copying the values of the collider
	margin.x = slime.child("margin").attribute("x").as_int();
	margin.y = slime.child("margin").attribute("y").as_int();
	colliderSize.x = slime.child("colliderSize").attribute("w").as_int();
	colliderSize.y = slime.child("colliderSize").attribute("h").as_int();
}

void j1Slime::Move(const std::vector<iPoint>* path, float dt)
{
	direction = App->path->CheckDirection(path);

	if (direction == Movement::DOWN_RIGHT)
	{
		animation = &diagonal_down;
		position.y += speed * dt;
		position.x += speed * dt;
		animation->flip = false;
	}

	else if (direction == Movement::DOWN_LEFT)
	{
		animation = &diagonal_down;
		position.y += speed * dt;
		position.x -= speed * dt;
		animation->flip = true;
	}

	else if (direction == Movement::UP_RIGHT)
	{
		animation = &diagonal_up;
		position.y -= speed * dt;
		position.x += speed * dt;
		animation->flip = false;
	}

	else if (direction == Movement::UP_LEFT)
	{
		animation = &diagonal_up;
		position.y -= speed * dt;
		position.x -= speed * dt;
		animation->flip = true;
	}

	else if (direction == Movement::DOWN)
	{
		animation = &down;
		position.y += speed * dt;
		animation->flip = false;
	}

	else if (direction == Movement::UP)
	{
		animation = &up;
		position.y -= speed * dt;
		animation->flip = false;
	}

	else if (direction == Movement::RIGHT)
	{
		animation = &lateral;
		animation->flip = false;
		position.x += speed * dt;
	}

	else if (direction == Movement::LEFT)
	{
		animation = &lateral;
		animation->flip = true;
		position.x -= speed * dt;
	}
}