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
	shotTimer.Start();

	collider = App->collisions->AddCollider({ (int)position.x + margin.x, (int)position.y + margin.y, colliderSize.x, colliderSize.y }, COLLIDER_ENEMY, App->entity);

	return true;
}

bool Exodus::Update(float dt, bool do_logic)
{
	BROFILER_CATEGORY("MindFlyerUpdate", Profiler::Color::LightSeaGreen)



	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN) {
		App->particles->AddParticle(App->particles->sword1, position.x, position.y, dt, COLLIDER_ENEMY_SHOT);
		App->particles->AddParticle(App->particles->sword1, position.x + 80, position.y, dt, COLLIDER_ENEMY_SHOT,0,90);
		state = 0;
	}
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		state = 1;

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
		}

		if (!receivedAbilityDamage && col_2->type == COLLIDER_ABILITY) {
			col_2->to_delete = true;
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
	pugi::xml_node mindflyer;
	mindflyer = config.child("mindflyer");

	// Copying the values of the collider
	margin.x = mindflyer.child("margin").attribute("x").as_int();
	margin.y = mindflyer.child("margin").attribute("y").as_int();
	colliderSize.x = mindflyer.child("colliderSize").attribute("w").as_int();
	colliderSize.y = mindflyer.child("colliderSize").attribute("h").as_int();

	// Copying combat values
	speed = mindflyer.attribute("speed").as_int();
	lifePoints = mindflyer.attribute("life").as_int();
}