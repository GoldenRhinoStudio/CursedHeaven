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

	height = 1;

	return true;
}

bool Exodus::Update(float dt, bool do_logic)
{
	BROFILER_CATEGORY("MindFlyerUpdate", Profiler::Color::LightSeaGreen)

	if (!App->entity->currentPlayer->attacking) receivedBasicDamage = false;
	if (!App->entity->currentPlayer->active_Q) receivedAbilityDamage = false;

	fPoint center = App->entity->currentPlayer->position;
	if (shotTimer.Read() > shotTime) {

		srand(time(NULL));
		attack = rand() % 4;
		attack = 3;
		switch (attack) {
		case 0:
			shotTime = 3000;
			App->particles->sword2.life = 1200;
			App->particles->AddParticleSpeed(App->particles->sword1, center.x - 180, center.y - 50, dt, COLLIDER_ENEMY_SHOT, 0, 180, { 0,speed });
			App->particles->AddParticleSpeed(App->particles->sword1, center.x - 120, center.y - 50, dt, COLLIDER_ENEMY_SHOT, 0, 180, { 0,speed });
			App->particles->AddParticleSpeed(App->particles->sword1, center.x - 60, center.y - 50, dt, COLLIDER_ENEMY_SHOT, 0, 180, { 0,speed });
			App->particles->AddParticleSpeed(App->particles->sword1, center.x, center.y - 50, dt, COLLIDER_ENEMY_SHOT, 0, 180, { 0,speed });
			App->particles->AddParticleSpeed(App->particles->sword1, center.x + 60, center.y - 50, dt, COLLIDER_ENEMY_SHOT, 0, 180, { 0,speed });
			App->particles->AddParticleSpeed(App->particles->sword1, center.x + 120, center.y - 50, dt, COLLIDER_ENEMY_SHOT, 0, 180, { 0,speed });
			break;
		case 1:
			shotTime = 3000;
			App->particles->sword2.life = 1500;
			App->particles->AddParticleSpeed(App->particles->sword1, center.x - 230, center.y, dt, COLLIDER_ENEMY_SHOT, 0, 90, { speed,0 });
			App->particles->AddParticleSpeed(App->particles->sword1, center.x - 230, center.y + 60, dt, COLLIDER_ENEMY_SHOT, 0, 90, { speed,0 });
			App->particles->AddParticleSpeed(App->particles->sword1, center.x - 230, center.y + 120, dt, COLLIDER_ENEMY_SHOT, 0, 90, { speed,0 });
			App->particles->AddParticleSpeed(App->particles->sword1, center.x - 230, center.y + 180, dt, COLLIDER_ENEMY_SHOT, 0, 90, { speed,0 });
			App->particles->AddParticleSpeed(App->particles->sword1, center.x - 230, center.y + 240, dt, COLLIDER_ENEMY_SHOT, 0, 90, { speed,0 });
			App->particles->AddParticleSpeed(App->particles->sword1, center.x - 230, center.y + 300, dt, COLLIDER_ENEMY_SHOT, 0, 90, { speed,0 });
			break;
		case 2:
			shotTime = 3000;
			App->particles->sword2.life = 1200;
			App->particles->AddParticleSpeed(App->particles->sword1, center.x - 120, center.y, dt, COLLIDER_ENEMY_SHOT, 0, 135, { (float)(-speed * cos(135 * DEGTORAD)) , (float)(speed * sin(135 * DEGTORAD)) });
			App->particles->AddParticleSpeed(App->particles->sword1, center.x + 60, center.y, dt, COLLIDER_ENEMY_SHOT, 0, -135, { (float)(speed * cos(135 * DEGTORAD)) , (float)(speed * sin(135 * DEGTORAD)) });
			break;
		case 3:
			shotTime = 8000;
			App->particles->vortex1.life = 10000;
			/*App->particles->AddParticleSpeed(App->particles->vortex1, center.x, center.y - 100, dt, COLLIDER_NONE, 0, 135, { 0,0 });
			App->particles->AddParticleSpeed(App->particles->vortex1, center.x, center.y - 100, dt, COLLIDER_NONE, 0, 45, { 0,0 });
			App->particles->AddParticleSpeed(App->particles->vortex1, center.x, center.y - 100, dt, COLLIDER_NONE, 0, 270, { 0,0 });
			App->particles->AddParticleSpeed(App->particles->vortex1, center.x, center.y - 100, dt, COLLIDER_NONE, 2000, 135, { 0,0 });*/
			App->particles->AddParticleSpeed(App->particles->vortex1, center.x, center.y - 100, dt, COLLIDER_ENEMY_SHOT, 2000, 45, { 0,0 });/*
			App->particles->AddParticleSpeed(App->particles->vortex1, center.x, center.y - 100, dt, COLLIDER_NONE, 2000, 270, { 0,0 });
			App->particles->AddParticleSpeed(App->particles->vortex1, center.x, center.y - 100, dt, COLLIDER_NONE, 4000, 135, { 0,0 });
			App->particles->AddParticleSpeed(App->particles->vortex1, center.x, center.y - 100, dt, COLLIDER_NONE, 4000, 45, { 0,0 });
			App->particles->AddParticleSpeed(App->particles->vortex1, center.x, center.y - 100, dt, COLLIDER_NONE, 4000, 270, { 0,0 });*/
			break;
		};
		shotTimer.Start();
	}	

	//App->map->EntityMovement(this);
	return true;
}

bool Exodus::DrawOrder(float dt) {

	// Drawing the harpy
	SDL_Rect* r = &animation->GetCurrentFrame(dt);

	Draw(r,false,0,0,1.0f,0,false,true);

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
	if (col_2->type == COLLIDER_ATTACK || col_2->type == COLLIDER_ABILITY) {

		if (!receivedBasicDamage && col_2->type == COLLIDER_ATTACK) {
			lifePoints -= App->entity->currentPlayer->basicDamage;
			App->audio->PlayFx(App->audio->boss_damage);
			receivedBasicDamage = true; 
		}

		if (!receivedAbilityDamage && col_2->type == COLLIDER_ABILITY) {
			if (App->entity->mage != nullptr)
				lifePoints -= App->entity->mage->fireDamage;
			App->audio->PlayFx(App->audio->boss_damage);
			receivedAbilityDamage = true;
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