#include "j1Turret.h"
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1FadeToBlack.h"
#include "j1BlackMage.h"
#include "j1Map.h"
#include "j1Scene1.h"
#include "j1Particles.h"
#include "j1Audio.h"

#include "Brofiler/Brofiler.h"

j1Turret::j1Turret(int x, int y, ENTITY_TYPES type) : j1Entity(x, y, ENTITY_TYPES::TURRET)
{
	animation = NULL;

	idle.LoadAnimation("idle", "turret", false);

	// Setting slime position
	initialPosition.x = position.x = x;
	initialPosition.y = position.y = y;
}

j1Turret::~j1Turret() {}

bool j1Turret::Start()
{
	LOG("Loading slime texture");

	sprites = App->tex->Load("textures/enemies/Turret/Turret.png");
	debug_tex = App->tex->Load("maps/path2.png");

	LoadProperties();

	animation = &idle;
	shotTimer.Start();
	height = 3;

	collider = App->collisions->AddCollider({ (int)position.x - margin.x, (int)position.y - margin.y, colliderSize.x, colliderSize.y }, COLLIDER_ENEMY, App->entity);

	return true;
}

bool j1Turret::Update(float dt, bool do_logic)
{
	BROFILER_CATEGORY("TurretUpdate", Profiler::Color::LightSeaGreen)

	if (!dead) {
		collider->SetPos(position.x + margin.x, position.y + margin.y);
		if (!App->entity->currentPlayer->attacking) receivedBasicDamage = false;
		if (!App->entity->currentPlayer->active_Q) receivedAbilityDamage = false;

		iPoint origin = { App->map->WorldToMap((int)position.x + colliderSize.x / 2, (int)position.y + colliderSize.y) };
		iPoint destination = { App->map->WorldToMap((int)App->entity->currentPlayer->collider->rect.x + App->entity->currentPlayer->collider->rect.w / 2, (int)App->entity->currentPlayer->collider->rect.y + App->entity->currentPlayer->collider->rect.h) };

		int distance = (int)sqrt(pow(destination.x - origin.x, 2) + pow(destination.y - origin.y, 2));

		if (distance <= ATTACK_RANGE_TURRET && App->entity->currentPlayer->collider->type == COLLIDER_PLAYER)
		{
			if (shotTimer.Read() >= lastTime_Shot + cooldown_Shot) {
				fPoint margin;
				margin.x = -5;
				margin.y = -5;

				fPoint edge;
				edge.x = App->entity->currentPlayer->collider->rect.x - (position.x + margin.x);
				edge.y = (position.y + margin.y) - App->entity->currentPlayer->collider->rect.y;

				// If the map is very big and its not enough accurate, we should use long double for the var angle
				double angle = -(atan2(edge.y, edge.x));

				fPoint speed_particle;
				fPoint p_speed = { 220, 220 };

				speed_particle.x = p_speed.x * cos(angle);
				speed_particle.y = p_speed.y * sin(angle);
				App->particles->mageShot.speed = speed_particle;

				App->particles->AddParticle(App->particles->mageShot, position.x + margin.x, position.y + margin.y, dt, COLLIDER_ENEMY_SHOT);
				//App->audio->PlayFx(App->audio->slime_attack);
				lastTime_Shot = shotTimer.Read();
			}
		}
	}
	
	return true;
}

bool j1Turret::DrawOrder(float dt) {

	// Drawing the slime
	SDL_Rect* r = &animation->GetCurrentFrame(dt);

	if (position.x - App->entity->currentPlayer->position.x >= 0)
		Draw(r, animation->flip, -10, -10);
	else
		Draw(r, animation->flip, -10, -10);
	return true;
}

bool j1Turret::CleanUp()
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

bool j1Turret::PostUpdate() {

	return true;
}

void j1Turret::OnCollision(Collider * col_1, Collider * col_2)
{
	if (App->entity->currentPlayer->receivedDamage == false && col_2->type == COLLIDER_PLAYER) {

		if (App->entity->player_type == KNIGHT && App->entity->currentPlayer->active_Q) {
			//App->audio->PlayFx(asdfasdfasdfasdfasf)
		}
		else {

			App->entity->currentPlayer->lifePoints -= App->entity->turret_Damage;
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
		}

		if (!receivedAbilityDamage && col_2->type == COLLIDER_ABILITY) {
			col_2->to_delete = true;
			if (App->entity->mage != nullptr)
				lifePoints -= App->entity->mage->fireDamage;
			App->audio->PlayFx(App->audio->slime_damage);

			receivedAbilityDamage = true;
		}

		if (lifePoints <= 0) {
			App->audio->PlayFx(App->audio->slime_death);
			dead = true;
			collider->to_delete = true;
			
			int item = rand() % 10 + 1;

			if (item > 7) {
				App->entity->AddItem(position.x - 10, position.y - 10, LIFE);
				App->entity->AddItem(position.x + 20, position.y, LIFE);
				App->entity->AddItem(position.x - 20, position.y + 20, LIFE);
			}
			else {
				App->entity->AddItem(position.x - 10, position.y, COIN);
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

bool j1Turret::Load(pugi::xml_node & data)
{
	return true;
}

bool j1Turret::Save(pugi::xml_node& data) const
{
	pugi::xml_node pos = data.append_child("position");

	pos.append_attribute("x") = position.x;
	pos.append_attribute("y") = position.y;

	return true;
}

void j1Turret::LoadProperties()
{
	pugi::xml_document config_file;
	config_file.load_file("config.xml");
	pugi::xml_node config;
	config = config_file.child("config");
	pugi::xml_node turret;
	turret = config.child("turret");

	// Copying the values of the collider
	margin.x = turret.child("margin").attribute("x").as_int();
	margin.y = turret.child("margin").attribute("y").as_int();
	colliderSize.x = turret.child("colliderSize").attribute("w").as_int();
	colliderSize.y = turret.child("colliderSize").attribute("h").as_int();

	lifePoints = turret.attribute("life").as_int();
	score = turret.attribute("score").as_int();
	cooldown_Shot = turret.child("combat").attribute("shotTime").as_uint();
	App->entity->turret_Damage = turret.child("combat").attribute("damage").as_int();
}