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
#include "j1Map.h"
#include "j1Scene1.h"

#include "Brofiler/Brofiler.h"

j1Slime::j1Slime(int x, int y, ENTITY_TYPES type) : j1Entity(x, y, ENTITY_TYPES::SLIME)
{
	animation = NULL;

	idle.LoadAnimation("idle", "harpy", false);
	move.LoadAnimation("move", "harpy", false);

	// Setting harpy position
	initialPosition.x = position.x = x;
	initialPosition.y = position.y = y;
}

j1Slime::~j1Slime() {}

bool j1Slime::Start()
{
	// Textures are loaded
	LOG("Loading harpy textures");
	sprites = App->tex->Load("textures/enemies/harpy/harpy.png");

	LoadProperties();

	animation = &idle;

	collider = App->collisions->AddCollider({ (int)position.x - margin.x, (int)position.y - margin.y, colliderSize.x, colliderSize.y }, COLLIDER_ENEMY, App->entity);

	return true;
}

bool j1Slime::Update(float dt, bool do_logic)
{
	BROFILER_CATEGORY("HarpyUpdate", Profiler::Color::LightSeaGreen)

	if (dead == false) {
		collider->SetPos(position.x, position.y);

		if (do_logic || path_created) {
			if ((App->entity->currentPlayer->position.x - position.x) <= DETECTION_RANGE && (App->entity->currentPlayer->position.x - position.x) >= -DETECTION_RANGE && App->entity->currentPlayer->collider->type == COLLIDER_PLAYER)
			{
				iPoint origin = { App->map->WorldToMap((int)position.x + colliderSize.x / 2, (int)position.y + colliderSize.y / 2) };
				iPoint destination;
				if (position.x < App->entity->currentPlayer->position.x)
					destination = { App->map->WorldToMap((int)App->entity->currentPlayer->position.x + App->entity->currentPlayer->playerSize.x + 1, (int)App->entity->currentPlayer->position.y + App->entity->currentPlayer->playerSize.y / 2) };
				else
					destination = { App->map->WorldToMap((int)App->entity->currentPlayer->position.x, (int)App->entity->currentPlayer->position.y + App->entity->currentPlayer->playerSize.y / 2) };

				if (App->path->IsWalkable(destination) && App->path->IsWalkable(origin) && App->entity->currentPlayer->dead == false)
				{
					path = App->path->CreatePath(origin, destination);
					Move(*path, dt);
					path_created = true;
				}
			}
			else if (path_created) {
				path->clear();
				path_created = false;
			}
		}

		if (App->entity->currentPlayer->position == App->entity->currentPlayer->initialPosition)
		{
			animation = &idle;
			position = initialPosition;
		}

		// Drawing the harpy
		SDL_Rect r = animation->GetCurrentFrame(dt);

		if (position.x - App->entity->currentPlayer->position.x >= 0)
			Draw(r, true, -10, -10);
		else
			Draw(r, false, -10, -10);
	}

	return true;
}

bool j1Slime::CleanUp()
{
	LOG("Unloading harpy");
	App->tex->UnLoad(sprites);
	if (collider != nullptr)
		collider->to_delete = true;

	if (path_created) {
		path->clear();
		path_created = false;
	}

	return true;
}

void j1Slime::OnCollision(Collider * col_1, Collider * col_2)
{
	if (col_2->type == COLLIDER_ATTACK || col_2->type == COLLIDER_SHOT) {
		App->entity->currentPlayer->score_points += 100;
		dead = true;
		collider->to_delete = true;
		/*int num = App->entity->entities.find(this);
		RELEASE(App->entity->entities.At(num)->data);
		App->entity->entities.remove(App->entity->entities.At(num));*/
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
	pugi::xml_node harpy;
	harpy = config.child("harpy");

	speed = harpy.attribute("speed").as_int();

	// Copying the values of the collider
	margin.x = harpy.child("margin").attribute("x").as_int();
	margin.y = harpy.child("margin").attribute("y").as_int();
	colliderSize.x = harpy.child("colliderSize").attribute("w").as_int();
	colliderSize.y = harpy.child("colliderSize").attribute("h").as_int();
}

void j1Slime::Move(std::vector<iPoint>& path, float dt)
{
	direction = App->path->CheckDirection(path);

	if (direction == Movement::DOWN_RIGHT)
	{
		animation = &move;
		position.y += speed * dt;
		position.x += speed * dt;
	}

	else if (direction == Movement::DOWN_LEFT)
	{
		animation = &move;
		position.y += speed * dt;
		position.x -= speed * dt;
	}

	else if (direction == Movement::UP_RIGHT)
	{
		animation = &move;
		position.y -= speed * dt;
		position.x += speed * dt;
	}

	else if (direction == Movement::UP_LEFT)
	{
		animation = &move;
		position.y -= speed * dt;
		position.x -= speed * dt;
	}

	else if (direction == Movement::DOWN)
	{
		animation = &move;
		position.y += speed * dt;
	}

	else if (direction == Movement::UP)
	{
		animation = &move;
		position.y -= speed * dt;
	}

	else if (direction == Movement::RIGHT)
	{
		animation = &move;
		position.x += speed * dt;
	}

	else if (direction == Movement::LEFT)
	{
		animation = &move;
		position.x -= speed * dt;
	}
}