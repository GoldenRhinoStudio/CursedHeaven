#include "j1MindFlyer.h"
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

j1MindFlyer::j1MindFlyer(int x, int y, ENTITY_TYPES type) : j1Entity(x, y, ENTITY_TYPES::MINDFLYER)
{
	animation = NULL;

	idle_diagonal_up.LoadAnimation("idleD_up", "mindflyer", false);
	idle_diagonal_down.LoadAnimation("idleD_down", "mindflyer", false);
	idle_lateral.LoadAnimation("idleLateral", "mindflyer", false);
	idle_down.LoadAnimation("idleDown", "mindflyer", false);
	idle_up.LoadAnimation("idleUp", "mindflyer", false);

	diagonal_up.LoadAnimation("diagonalUp", "mindflyer", false);
	diagonal_down.LoadAnimation("diagonalDown", "mindflyer", false);
	lateral.LoadAnimation("lateral", "mindflyer", false);
	up.LoadAnimation("up", "mindflyer", false);
	down.LoadAnimation("down", "mindflyer", false);

	// Setting harpy position
	initialPosition.x = position.x = x;
	initialPosition.y = position.y = y;
}

j1MindFlyer::~j1MindFlyer() {}

bool j1MindFlyer::Start()
{
	// Textures are loaded
	LOG("Loading mindflyer texture");
	sprites = App->tex->Load("textures/enemies/bosses/mindflyer.png");
	debug_tex = App->tex->Load("maps/path2.png");

	LoadProperties();

	animation = &idle_down;
	shotTimer.Start();
	supershotTimer.Start();

	collider = App->collisions->AddCollider({ (int)position.x + margin.x, (int)position.y + margin.y, colliderSize.x, colliderSize.y }, COLLIDER_ENEMY, App->entity);

	return true;
}

bool j1MindFlyer::Update(float dt, bool do_logic)
{
	BROFILER_CATEGORY("MindFlyerUpdate", Profiler::Color::LightSeaGreen)

	if (!dead) {
		collider->SetPos(position.x + margin.x, position.y + margin.y);
		if (!App->entity->currentPlayer->attacking) receivedBasicDamage = false;
		if (!App->entity->currentPlayer->active_Q) receivedAbilityDamage = false;

		iPoint origin = { App->map->WorldToMap((int)position.x + colliderSize.x / 2, (int)position.y + colliderSize.y) };
		iPoint destination = { App->map->WorldToMap((int)App->entity->currentPlayer->position.x + App->entity->currentPlayer->playerSize.x + 1, (int)App->entity->currentPlayer->position.y + App->entity->currentPlayer->playerSize.y) };

		int distance = (int)sqrt(pow(destination.x - origin.x, 2) + pow(destination.y - origin.y, 2));

		if (distance <= MF_DETECTION_RANGE && App->entity->currentPlayer->collider->type == COLLIDER_PLAYER)
		{

			if (App->entity->currentPlayer->dead == false)
			{
				if (do_logic) {
					/*if (path != nullptr)
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
					if (distance <= ATTACK_RANGE_MF || spinningShot) {
						if ((shotTimer.Read() >= lastTime_Shot + cooldown_Shot) && loopAngle == 320) {

							fPoint speed_particle[8];
							fPoint particle_speed = { 250, 250 };
							
							speed_particle[0].x = particle_speed.x * cos(0 * DEGTORAD);
							speed_particle[0].y = particle_speed.y * sin(0 * DEGTORAD);
							App->particles->mindflyerAttack.speed = speed_particle[0];
							App->particles->AddParticle(App->particles->mindflyerAttack, position.x, position.y, dt, COLLIDER_ENEMY_SHOT);
							
							speed_particle[1].x = particle_speed.x * cos(180 * DEGTORAD);
							speed_particle[1].y = particle_speed.y * sin(180 * DEGTORAD);
							App->particles->mindflyerAttack.speed = speed_particle[1];
							App->particles->AddParticle(App->particles->mindflyerAttack, position.x, position.y, dt, COLLIDER_ENEMY_SHOT);
														
							speed_particle[2].x = particle_speed.x * cos(-45 * DEGTORAD);
							speed_particle[2].y = particle_speed.y * sin(-45 * DEGTORAD);
							App->particles->mindflyerAttack.speed = speed_particle[2];
							App->particles->AddParticle(App->particles->mindflyerAttack, position.x, position.y, dt, COLLIDER_ENEMY_SHOT);
							
							speed_particle[3].x = particle_speed.x * cos(-135 * DEGTORAD);
							speed_particle[3].y = particle_speed.y * sin(-135 * DEGTORAD);
							App->particles->mindflyerAttack.speed = speed_particle[3];
							App->particles->AddParticle(App->particles->mindflyerAttack, position.x, position.y, dt, COLLIDER_ENEMY_SHOT);
														
							speed_particle[4].x = particle_speed.x * cos(-315 * DEGTORAD);
							speed_particle[4].y = particle_speed.y * sin(-315 * DEGTORAD);
							App->particles->mindflyerAttack.speed = speed_particle[4];
							App->particles->AddParticle(App->particles->mindflyerAttack, position.x, position.y, dt, COLLIDER_ENEMY_SHOT);
							
							speed_particle[5].x = particle_speed.x * cos(-225 * DEGTORAD);
							speed_particle[5].y = particle_speed.y * sin(-225 * DEGTORAD);
							App->particles->mindflyerAttack.speed = speed_particle[5];
							App->particles->AddParticle(App->particles->mindflyerAttack, position.x, position.y, dt, COLLIDER_ENEMY_SHOT);
							
							speed_particle[6].x = particle_speed.x * cos(-90 * DEGTORAD);
							speed_particle[6].y = particle_speed.y * sin(-90 * DEGTORAD);
							App->particles->mindflyerAttack.speed = speed_particle[6];
							App->particles->AddParticle(App->particles->mindflyerAttack, position.x, position.y, dt, COLLIDER_ENEMY_SHOT);

							speed_particle[7].x = particle_speed.x * cos(-270 * DEGTORAD);
							speed_particle[7].y = particle_speed.y * sin(-270 * DEGTORAD);
							App->particles->mindflyerAttack.speed = speed_particle[7];
							App->particles->AddParticle(App->particles->mindflyerAttack, position.x, position.y, dt, COLLIDER_ENEMY_SHOT);

							App->audio->PlayFx(App->audio->boss_attack);
							lastTime_Shot = shotTimer.Read();
						}

						if (supershotTimer.Read() >= lastTime_Supershot + 10000) {

							spinningShot = true;

							if (loopAngle % 5 == 0) {

								fPoint speed_particle[8];
								fPoint particle_speed = { 200, 200 };

								speed_particle[0].x = particle_speed.x * cos((0 + loopAngle) * DEGTORAD);
								speed_particle[0].y = particle_speed.y * sin((0 + loopAngle) * DEGTORAD);
								App->particles->mindflyerAttack.speed = speed_particle[0];
								App->particles->AddParticle(App->particles->mindflyerAttack, position.x, position.y, dt, COLLIDER_ENEMY_SHOT);

								speed_particle[1].x = particle_speed.x * cos((180 + loopAngle) * DEGTORAD);
								speed_particle[1].y = particle_speed.y * sin((180 + loopAngle) * DEGTORAD);
								App->particles->mindflyerAttack.speed = speed_particle[1];
								App->particles->AddParticle(App->particles->mindflyerAttack, position.x, position.y, dt, COLLIDER_ENEMY_SHOT);
								
								speed_particle[6].x = particle_speed.x * cos((-90 + loopAngle) * DEGTORAD);
								speed_particle[6].y = particle_speed.y * sin((-90 + loopAngle) * DEGTORAD);
								App->particles->mindflyerAttack.speed = speed_particle[6];
								App->particles->AddParticle(App->particles->mindflyerAttack, position.x, position.y, dt, COLLIDER_ENEMY_SHOT);

								speed_particle[7].x = particle_speed.x * cos((-270 + loopAngle) * DEGTORAD);
								speed_particle[7].y = particle_speed.y * sin((-270 + loopAngle) * DEGTORAD);
								App->particles->mindflyerAttack.speed = speed_particle[7];
								App->particles->AddParticle(App->particles->mindflyerAttack, position.x, position.y, dt, COLLIDER_ENEMY_SHOT);
							}

							loopAngle++;
							int a = loopAngle;
							int b = a;

							if (loopAngle >= 320 + 360) {
								loopAngle = 320;
								lastTime_Supershot = supershotTimer.Read();
								spinningShot = false;
							}
						}
					}
					else if (loopAngle == 320){
						Move(path, dt);
					}
				}
			}
		}
		else {
			/*if (path != nullptr)
				path->clear();*/
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

bool j1MindFlyer::DrawOrder(float dt) {

	// Drawing the harpy
	SDL_Rect* r = &animation->GetCurrentFrame(dt);

	if (position.x > App->entity->currentPlayer->position.x)
		Draw(r, true, -10, -10, 1.2f);
	else
		Draw(r, false, -10, -10, 1.2f);
	return true;
}

bool j1MindFlyer::CleanUp()
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

bool j1MindFlyer::PostUpdate() {
	return true;
}

void j1MindFlyer::OnCollision(Collider * col_1, Collider * col_2)
{
	if (App->entity->currentPlayer->receivedDamage == false && col_2->type == COLLIDER_PLAYER) {

		if (App->entity->player_type == KNIGHT && App->entity->currentPlayer->active_Q) {
			//App->audio->PlayFx(asdfasdfasdfasdfasf)
		}
		else {

			App->entity->currentPlayer->lifePoints -= App->entity->mindflyer_Damage;
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
			
			App->entity->currentPlayer->coins += 20;

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

bool j1MindFlyer::Load(pugi::xml_node & data)
{

	return true;
}

bool j1MindFlyer::Save(pugi::xml_node& data) const
{
	pugi::xml_node pos = data.append_child("position");
	pos.append_attribute("x") = App->map->WorldToMap(position.x, position.y).x;
	pos.append_attribute("y") = App->map->WorldToMap(position.x, position.y).y;

	pugi::xml_node stats = data.append_child("stats");
	stats.append_attribute("life") = lifePoints;
	stats.append_attribute("dead") = dead;

	return true;
}

void j1MindFlyer::LoadProperties()
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
	if (App->entity->loadingMf) {
		lifePoints = App->entity->mf_lifePoints; 
		App->entity->loadingMf = false;
	}else
		lifePoints = mindflyer.attribute("life").as_int();

	speed = mindflyer.attribute("speed").as_int();
	score = mindflyer.attribute("score").as_int();
	cooldown_Shot = mindflyer.child("combat").attribute("shotTime").as_uint();
	App->entity->mindflyer_Damage = mindflyer.child("combat").attribute("damage").as_int();
}

void j1MindFlyer::Move(const std::vector<iPoint>* path, float dt)
{
	iPoint pos = { collider->rect.x + collider->rect.w/2, collider->rect.y + collider->rect.h};
	if (App->path->check_nextTile(path, &node, &pos, direction))
		node++;
	direction = App->path->CheckDirection(path,&node);

	if (direction == Movement::DOWN_RIGHT)
	{
		animation = &diagonal_down;
		position.y += speed * dt;
		position.x += speed * dt;
	}

	else if (direction == Movement::DOWN_LEFT)
	{
		animation = &diagonal_down;
		position.y += speed * dt;
		position.x -= speed * dt;
	}

	else if (direction == Movement::UP_RIGHT)
	{
		animation = &diagonal_up;
		position.y -= speed * dt;
		position.x += speed * dt;
	}

	else if (direction == Movement::UP_LEFT)
	{
		animation = &diagonal_up;
		position.y -= speed * dt;
		position.x -= speed * dt;
	}

	else if (direction == Movement::DOWN)
	{
		animation = &down;
		position.y += speed * dt;
	}

	else if (direction == Movement::UP)
	{
		animation = &up;
		position.y -= speed * dt;
	}

	else if (direction == Movement::RIGHT)
	{
		animation = &lateral;
		position.x += speed * dt;
	}

	else if (direction == Movement::LEFT)
	{
		animation = &lateral;
		position.x -= speed * dt;
	}
}