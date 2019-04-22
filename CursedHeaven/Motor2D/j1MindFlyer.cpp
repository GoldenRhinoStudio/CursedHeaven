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

#include "Brofiler/Brofiler.h"

j1MindFlyer::j1MindFlyer(int x, int y, ENTITY_TYPES type) : j1Entity(x, y, ENTITY_TYPES::SLIME)
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
	sprites;
	debug_tex = App->tex->Load("maps/path2.png");

	LoadProperties();

	animation = &idle_down;

	collider = App->collisions->AddCollider({ (int)position.x - margin.x, (int)position.y - margin.y, colliderSize.x, colliderSize.y }, COLLIDER_ENEMY, App->entity);

	return true;
}

bool j1MindFlyer::Update(float dt, bool do_logic)
{
	BROFILER_CATEGORY("MindFlyerUpdate", Profiler::Color::LightSeaGreen)

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
					if (path != nullptr)
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
					if (distance <= ATTACK_RANGE_MF) {
						if (shotTimer.Read() >= lastTime_Shot + cooldown_Shot) {
							int x = App->entity->currentPlayer->position.x;
							int y = App->entity->currentPlayer->position.y;

							// To change where the particle is born
							fPoint margin;
							margin.x = 8;
							margin.y = 8;

							fPoint edge;
							edge.x = x - (position.x + margin.x) - (App->render->camera.x / (int)App->win->GetScale());
							edge.y = (position.y + margin.y) - y + (App->render->camera.y / (int)App->win->GetScale());

							// If the map is very big and its not enough accurate, we should use long double for the var angle
							double angle = -(atan2(edge.y, edge.x));

							fPoint speed_particle;
							fPoint p_speed = { 300, 300 };

							speed_particle.x = p_speed.x * cos(angle);
							speed_particle.y = p_speed.y * sin(angle);
							App->particles->shot_right.speed = speed_particle;

							App->particles->AddParticle(App->particles->shot_right, position.x + margin.x, position.y + margin.y, dt, COLLIDER_ENEMY_SHOT);

							lastTime_Shot = shotTimer.Read();
						}
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

bool j1MindFlyer::DrawOrder(float dt) {

	// Drawing the harpy
	SDL_Rect* r = &animation->GetCurrentFrame(dt);

	if (position.x - App->entity->currentPlayer->position.x >= 0)
		Draw(r, animation->flip, -10, -10, 1.2f);
	else
		Draw(r, animation->flip, -10, -10, 1.2f);
	return true;
}

bool j1MindFlyer::CleanUp()
{
	LOG("Unloading mindflyer");
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

bool j1MindFlyer::PostUpdate() {
	return true;
}

void j1MindFlyer::OnCollision(Collider * col_1, Collider * col_2)
{
	if (col_2->type == COLLIDER_ATTACK || col_2->type == COLLIDER_ABILITY) {

		if (!receivedBasicDamage && col_2->type == COLLIDER_ATTACK) {
			lifePoints -= App->entity->currentPlayer->basicDamage;
			receivedBasicDamage = true;
		}

		if (!receivedAbilityDamage && col_2->type == COLLIDER_ABILITY) {
			if (App->entity->mage != nullptr)
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

bool j1MindFlyer::Load(pugi::xml_node & data)
{
	return true;
}

bool j1MindFlyer::Save(pugi::xml_node& data) const
{
	pugi::xml_node pos = data.append_child("position");

	pos.append_attribute("x") = position.x;
	pos.append_attribute("y") = position.y;

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
	speed = mindflyer.attribute("speed").as_int();
	lifePoints = mindflyer.attribute("life").as_int();
	cooldown_Shot = mindflyer.attribute("shotTime").as_uint();
}

void j1MindFlyer::Move(const std::vector<iPoint>* path, float dt)
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