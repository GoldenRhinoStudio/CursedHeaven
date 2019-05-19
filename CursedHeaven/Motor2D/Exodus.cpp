#include "Exodus.h"
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
#include "j1Particles.h"
#include "j1Audio.h"
#include <time.h>

#include "Brofiler/Brofiler.h"


Exodus::Exodus(int x, int y, ENTITY_TYPES type) : j1Entity(x, y, ENTITY_TYPES::EXODUS)
{
	animation = NULL;

	idle.LoadAnimation("idle", "exodus", false);

	// Setting harpy position
	initialPosition.x = position.x = x;
	initialPosition.y = position.y = y;
}


Exodus::~Exodus(){}

bool Exodus::Start()
{
	// Textures are loaded
	sprites = App->tex->Load("textures/enemies/bosses/FinalBoss.png");

	LoadProperties();

	animation = &idle;

	collider = App->collisions->AddCollider({ (int)position.x + margin.x, (int)position.y + margin.y, colliderSize.x, colliderSize.y }, COLLIDER_ENEMY, App->entity);

	return true;
}

bool Exodus::Update(float dt, bool do_logic)
{
	BROFILER_CATEGORY("MindFlyerUpdate", Profiler::Color::LightSeaGreen)

	if (!App->entity->currentPlayer->attacking) receivedBasicDamage = false;
	if (!App->entity->currentPlayer->active_Q) receivedAbilityDamage = false;

	fPoint center = { position.x + animation->frames->w / 2, position.y + animation->frames->h / 2 };
	if (shotTimer.Read() > shotTime) {

		srand(time(NULL));
		attack = rand() % 3;

		switch (attack) {
		case 0:
			App->particles->AddParticleSpeed(App->particles->sword1, center.x - 120, center.y - 50, dt, COLLIDER_ENEMY_SHOT, 0, 180, { 0,250 });
			App->particles->AddParticleSpeed(App->particles->sword1, center.x - 60, center.y - 50, dt, COLLIDER_ENEMY_SHOT, 0, 180, { 0,250 });
			App->particles->AddParticleSpeed(App->particles->sword1, center.x, center.y - 50, dt, COLLIDER_ENEMY_SHOT, 0, 180, { 0,250 });
			App->particles->AddParticleSpeed(App->particles->sword1, center.x + 60, center.y - 50, dt, COLLIDER_ENEMY_SHOT, 0, 180, { 0,250 });
			break;
		case 1:
			App->particles->AddParticleSpeed(App->particles->sword1, center.x - 130, center.y + 60, dt, COLLIDER_ENEMY_SHOT, 0, 90, { 250,0 });
			App->particles->AddParticleSpeed(App->particles->sword1, center.x - 130, center.y + 120, dt, COLLIDER_ENEMY_SHOT, 0, 90, { 250,0 });
			App->particles->AddParticleSpeed(App->particles->sword1, center.x - 130, center.y + 180, dt, COLLIDER_ENEMY_SHOT, 0, 90, { 250,0 });
			App->particles->AddParticleSpeed(App->particles->sword1, center.x - 130, center.y + 240, dt, COLLIDER_ENEMY_SHOT, 0, 90, { 250,0 });
			break;
		case 2:
			App->particles->AddParticleSpeed(App->particles->sword1, center.x - 120, center.y, dt, COLLIDER_ENEMY_SHOT, 0, 135, { (float)(-250 * cos(135 * DEGTORAD)) , (float)(250 * sin(135 * DEGTORAD)) });
			App->particles->AddParticleSpeed(App->particles->sword1, center.x + 60, center.y, dt, COLLIDER_ENEMY_SHOT, 0, -135, { (float)(250 * cos(135 * DEGTORAD)) , (float)(250 * sin(135 * DEGTORAD)) });
			break;
		};
		shotTimer.Start();
	}


	

	App->map->EntityMovement(this);
	return true;
}

bool Exodus::DrawOrder(float dt) {

	// Drawing the harpy
	SDL_Rect* r = &animation->GetCurrentFrame(dt);

	Draw(r);

	return true;
}

bool Exodus::CleanUp()
{
	LOG("Unloading mindflyer");
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

bool Exodus::PostUpdate() {
	return true;
}

void Exodus::OnCollision(Collider * col_1, Collider * col_2)
{
	/*if (col_2->type == COLLIDER_ATTACK)
	{
		for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
		{
			if (App->particles->active[i] != nullptr)
			{
				delete App->particles->active[i];
				App->particles->active[i] = nullptr;
			}
		}
	}*/

	if (col_2->type == COLLIDER_ATTACK || col_2->type == COLLIDER_ABILITY) {

		if (!receivedBasicDamage && col_2->type == COLLIDER_ATTACK) {
			if (App->entity->player_type == MAGE) col_2->to_delete = true;
			lifePoints -= App->entity->currentPlayer->basicDamage;
			App->audio->PlayFx(App->audio->boss_damage);
			receivedBasicDamage = true; 
			if (shotTimer.Read() == 0) {//attack when receive first damage
				shotTimer.Start();
			}
		}

		if (!receivedAbilityDamage && col_2->type == COLLIDER_ABILITY) {
			col_2->to_delete = true;
			if (App->entity->mage != nullptr)
				lifePoints -= App->entity->mage->fireDamage;
			App->audio->PlayFx(App->audio->boss_damage);

			receivedAbilityDamage = true; 
			if (shotTimer.Read() == 0) {//attack when receive first damage
				shotTimer.Start();
			}
		}

		if (lifePoints <= 0) {
			App->audio->PlayFx(App->audio->boss_death);
			dead = true;
			App->entity->currentPlayer->victory = true;
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

bool Exodus::Load(pugi::xml_node & data)
{
	return true;
}

bool Exodus::Save(pugi::xml_node& data) const
{
	pugi::xml_node pos = data.append_child("position");

	pos.append_attribute("x") = position.x;
	pos.append_attribute("y") = position.y;

	return true;
}

void Exodus::LoadProperties()
{
	pugi::xml_document config_file;
	config_file.load_file("config.xml");
	pugi::xml_node config;
	config = config_file.child("config");
	pugi::xml_node exodus;
	exodus = config.child("exodus");

	// Copying the values of the collider
	margin.x = exodus.child("margin").attribute("x").as_int();
	margin.y = exodus.child("margin").attribute("y").as_int();
	colliderSize.x = exodus.child("colliderSize").attribute("w").as_int();
	colliderSize.y = exodus.child("colliderSize").attribute("h").as_int();

	// Copying combat values
	speed = exodus.attribute("speed").as_int();
	lifePoints = exodus.attribute("life").as_int();
	shotTime = exodus.child("combat").attribute("shotTime").as_int();
	damage = exodus.child("combat").attribute("damage").as_int();
}