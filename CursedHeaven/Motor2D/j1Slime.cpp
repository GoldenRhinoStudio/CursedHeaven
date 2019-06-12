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
#include "j1Audio.h"
#include "j1Particles.h"
#include <time.h>

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

	// Setting slime position
	initialPosition.x = position.x = x;
	initialPosition.y = position.y = y;
}

j1Slime::~j1Slime() {}

bool j1Slime::Start()
{
	LOG("Loading slime texture");

	sprites = App->tex->Load("textures/enemies/Slimes/Slime.png");
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
		if (!App->entity->currentPlayer->attacking) receivedBasicDamage = false;
		if (!App->entity->currentPlayer->active_Q) receivedAbilityDamage = false; 
		
		iPoint origin = { App->map->WorldToMap((int)position.x + colliderSize.x / 2, (int)position.y + colliderSize.y) };
		iPoint destination = { App->map->WorldToMap((int)App->entity->currentPlayer->collider->rect.x, (int)App->entity->currentPlayer->collider->rect.y + App->entity->currentPlayer->collider->rect.h/2) };

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
					if (distance <= ATTACK_RANGE_SLIME) {
						App->audio->PlayFx(App->audio->slime_attack);
					}
					Move(path, dt);
				}
				else if (!target_found && path != nullptr) {

				}
				//fix attack
			}
		}
		else {
			/*	if (path != nullptr)
			path->clear();*/
			target_found = false;
		}

		if (App->entity->currentPlayer->position == App->entity->currentPlayer->initialPosition)
		{
			animation = &idle_down;
			position = initialPosition;
		}

	}

	App->map->EntityMovementTest(this);
	collider->SetPos(position.x + margin.x, position.y + margin.y);
	
	return true;
}

bool j1Slime::DrawOrder(float dt) {

	// Drawing the slime
	SDL_Rect* r = &animation->GetCurrentFrame(dt);

	if (position.x - App->entity->currentPlayer->position.x >= 0)
		Draw(r, animation->flip, -10, -10);
	else
		Draw(r, animation->flip, -10, -10);
	return true;
}

bool j1Slime::CleanUp()
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

bool j1Slime::PostUpdate() {

	return true;
}

void j1Slime::OnCollision(Collider * col_1, Collider * col_2)
{
	if (App->entity->currentPlayer->receivedDamage == false && col_2->type == COLLIDER_PLAYER) {
		
		if (App->entity->player_type == KNIGHT && App->entity->currentPlayer->active_Q) {
			//App->audio->PlayFx(asdfasdfasdfasdfasf)
		}
		else{

			App->entity->currentPlayer->lifePoints -= App->entity->slime_Damage;
			App->entity->currentPlayer->receivedDamage = true;

			if (App->entity->player_type == MAGE) App->audio->PlayFx(App->audio->damage_bm);
			else App->audio->PlayFx(App->audio->damage_dk);
		}

		if (App->entity->currentPlayer->lifePoints <= 0) 
			App->entity->currentPlayer->dead = true;
	}

	if (col_2->type == COLLIDER_ATTACK || col_2->type == COLLIDER_ABILITY) {
		
		if (!receivedBasicDamage && col_2->type == COLLIDER_ATTACK) {
			if (App->entity->player_type == MAGE) col_2->to_delete = true;

			lifePoints -= App->entity->currentPlayer->basicDamage;
			App->audio->PlayFx(App->audio->slime_damage);
			receivedBasicDamage = true;

			if (lifePoints > 0 && attacking == false && App->entity->player_type == KNIGHT) {

				if ((animation == &lateral || animation == &idle_lateral) && position.x - App->entity->currentPlayer->position.x >= 0)
				position.x += App->entity->currentPlayer->knockback;
				else if (animation == &lateral || animation == &idle_lateral)
					position.x -= App->entity->currentPlayer->knockback;
				else if (animation == &up || animation == &idle_up)
					position.y += App->entity->currentPlayer->knockback;
				else if (animation == &down || animation == &idle_down)
					position.y -= App->entity->currentPlayer->knockback;
				else if ((animation == &diagonal_up || animation == &idle_diagonal_up) && position.x - App->entity->currentPlayer->position.x >= 0) {
					position.x += App->entity->currentPlayer->knockback;
					position.y += 7;
				}
				else if (animation == &diagonal_up || animation == &idle_diagonal_up) {
					position.x -= App->entity->currentPlayer->knockback;
					position.y += 7;
				}
				else if ((animation == &diagonal_down || animation == &idle_diagonal_down) && position.x - App->entity->currentPlayer->position.x >= 0) {
					position.x += App->entity->currentPlayer->knockback;
					position.y -= 7;
				}
				else if (animation == &diagonal_down || animation == &idle_diagonal_down) {
					position.x -= App->entity->currentPlayer->knockback;
					position.y -= 7;
				}
			}
			else if (lifePoints > 0 && attacking == false && App->entity->player_type == MAGE) {
				if ((animation == &lateral || animation == &idle_lateral) && position.x - App->entity->currentPlayer->position.x >= 0)
					position.x += App->entity->currentPlayer->knockback;
				else if (animation == &lateral || animation == &idle_lateral)
					position.x -= App->entity->currentPlayer->knockback;
				else if (animation == &up || animation == &idle_up)
					position.y += App->entity->currentPlayer->knockback;
				else if (animation == &down || animation == &idle_down)
					position.y -= App->entity->currentPlayer->knockback;
				else if ((animation == &diagonal_up || animation == &idle_diagonal_up) && position.x - App->entity->currentPlayer->position.x >= 0) {
					position.x += App->entity->currentPlayer->knockback;
					position.y += 5;
				}
				else if (animation == &diagonal_up || animation == &idle_diagonal_up) {
					position.x -= App->entity->currentPlayer->knockback;
					position.y += 5;
				}
				else if ((animation == &diagonal_down || animation == &idle_diagonal_down) && position.x - App->entity->currentPlayer->position.x >= 0) {
					position.x += App->entity->currentPlayer->knockback;
					position.y -= 5;
				}
				else if (animation == &diagonal_down || animation == &idle_diagonal_down) {
					position.x -= App->entity->currentPlayer->knockback;
					position.y -= 5;
				}
			}
		}

		if (!receivedAbilityDamage && col_2->type == COLLIDER_ABILITY) {
			col_2->to_delete = true;
			if(App->entity->mage != nullptr)
				lifePoints -= App->entity->mage->fireDamage;
			App->audio->PlayFx(App->audio->slime_damage);

			receivedAbilityDamage = true;
		}

		if (lifePoints <= 0) {
			App->audio->PlayFx(App->audio->slime_death);
			dead = true;
			collider->to_delete = true;

			srand(time(NULL));
			int item = rand() % 10 + 1;

			if (item > 7) {
				App->entity->AddItem(position.x - 5, position.y - 5, LIFE);
				App->entity->AddItem(position.x + 5, position.y, LIFE);
				App->entity->AddItem(position.x - 5, position.y + 5, LIFE);
			}
			else {
				App->entity->AddItem(position.x - 5, position.y, COIN);
			}

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
	pos.append_attribute("x") = App->map->WorldToMap(position.x, position.y).x;
	pos.append_attribute("y") = App->map->WorldToMap(position.x, position.y).y;

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

	// Copying the values of the collider
	margin.x = slime.child("margin").attribute("x").as_int();
	margin.y = slime.child("margin").attribute("y").as_int();
	colliderSize.x = slime.child("colliderSize").attribute("w").as_int();
	colliderSize.y = slime.child("colliderSize").attribute("h").as_int();

	speed = slime.attribute("speed").as_float();
	lifePoints = slime.attribute("life").as_int();
	score = slime.attribute("score").as_int();
	App->entity->slime_Damage = slime.child("combat").attribute("damage").as_int();

}

void j1Slime::Move(const std::vector<iPoint>* path, float dt)
{
	/*for (uint i = 0; i < path->size(); ++i)
	{
		iPoint pos = App->map->MapToWorld(path->at(i).x, path->at(i).y);
		App->render->Blit(debug_tex, pos.x, pos.y);
	}*/ //blit path

	iPoint pos = App->map->WorldToMap((int)collider->rect.x + (int)collider->rect.w/2, (int)collider->rect.y + (int)collider->rect.h);

	direction = App->path->CheckDirection(path, &node);

	if (App->path->check_nextTile(path, &node, &pos,direction)) {
		node++;
	}

	fPoint next_position = position;

	
	if (direction == Movement::DOWN_RIGHT)
	{
		animation = &diagonal_down;
		next_position.y += (speed * dt) / 2;
		next_position.x += speed * dt;
		animation->flip = false;
	}
	else if (direction == Movement::DOWN_LEFT)
	{
		animation = &diagonal_down;
		next_position.y += (speed * dt) / 2;
		next_position.x -= speed * dt;
		animation->flip = true;
	}
	else if (direction == Movement::UP_RIGHT)
	{
		animation = &diagonal_up;
		next_position.y -= (speed * dt) / 2;
		next_position.x += speed * dt;
		animation->flip = false;
	}
	else if (direction == Movement::UP_LEFT)
	{
		animation = &diagonal_up;
		next_position.y -= (speed * dt) / 2;
		next_position.x -= speed * dt;
		animation->flip = true;
	}
	else if (direction == Movement::DOWN)
	{
		animation = &down;
		next_position.y += speed * dt;
		animation->flip = false;
	}
	else if (direction == Movement::UP)
	{
		animation = &up;
		next_position.y -= speed * dt;
		animation->flip = false;
	}
	else if (direction == Movement::RIGHT)
	{
		animation = &lateral;
		animation->flip = false;
		next_position.x += speed * dt;
	}
	else if (direction == Movement::LEFT)
	{
		animation = &lateral;
		animation->flip = true;
		next_position.x -= speed * dt;
	}

	if (App->path->IsWalkable(App->map->WorldToMap((int)next_position.x, (int)next_position.y)))
		position = next_position;
}