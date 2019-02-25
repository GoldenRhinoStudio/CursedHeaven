#include "j1Skeleton.h"
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

#define GRAVITY 1

j1Skeleton::j1Skeleton(int x, int y, ENTITY_TYPES type) : j1Entity(x, y, ENTITY_TYPES::SKELETON)
{
	animation = NULL;

	idle.LoadEnemyAnimations("idle", "skeleton");
	move.LoadEnemyAnimations("move", "skeleton");

	// Setting skeleton position
	initialPosition.x = position.x = x;
	initialPosition.y = position.y = y;
}

j1Skeleton::~j1Skeleton() {}

bool j1Skeleton::Start()
{
	// Textures are loaded
	LOG("Loading skeleton textures");
	sprites = App->tex->Load("textures/enemies/skeleton/zombie&skeleton.png");

	LoadSkeletonProperties();

	animation = &idle;

	collider = App->collisions->AddCollider({ (int)position.x - margin.x, (int)position.y - margin.y, colliderSize.x, colliderSize.y }, COLLIDER_ENEMY, App->entity);

	return true;
}

bool j1Skeleton::Update(float dt, bool do_logic)
{
	BROFILER_CATEGORY("SkeletonUpdate", Profiler::Color::LightSeaGreen)

	if (dead == false) {
		collider->SetPos(position.x, position.y);

		position.y += GRAVITY + GRAVITY * dt;

		if (do_logic || path_created) {
			if ((App->entity->player->position.x - position.x) <= DETECTION_RANGE && (App->entity->player->position.x - position.x) >= -DETECTION_RANGE && App->entity->player->collider->type == COLLIDER_PLAYER && !dead)
			{
				iPoint origin = { App->map->WorldToMap((int)position.x + colliderSize.x / 2, (int)position.y + colliderSize.y / 2) };
				iPoint destination;
				if (position.x < App->entity->player->position.x)
					destination = { App->map->WorldToMap((int)App->entity->player->position.x + App->entity->player->playerSize.x + 1, (int)App->entity->player->position.y + App->entity->player->playerSize.y / 2) };
				else
					destination = { App->map->WorldToMap((int)App->entity->player->position.x, (int)App->entity->player->position.y + App->entity->player->playerSize.y / 2) };

				if (App->path->IsWalkable(destination) && App->path->IsWalkable(origin))
				{
					path = App->path->CreatePath(origin, destination);
					Move(*path, dt);
					path_created = true;
				}
			}
			else if (path_created) {
				path->Clear();
				path_created = false;
			}
		}

		if (App->entity->player->position == App->entity->player->initialPosition)
		{
			animation = &idle;
			position = initialPosition;
		}

		// Blitting the skeleton
		SDL_Rect r = animation->GetCurrentFrame(dt);

		if (position.x - App->entity->player->position.x >= 0)
			Draw(r, false, 0, 0);
		else
			Draw(r, true, 0, 0);
	}

	return true;
}

bool j1Skeleton::CleanUp()
{
	LOG("Unloading skeleton");
	App->tex->UnLoad(sprites);
	if (collider != nullptr)
		collider->to_delete = true;

	if (path_created) {
		path->Clear();
		path_created = false;
	}

	return true;
}

void j1Skeleton::OnCollision(Collider * col_1, Collider * col_2)
{
	COLLISION_DIRECTION direction;

	if (col_2->type == COLLIDER_WALL)
	{
		direction = col_1->CheckDirection(col_2->rect);
	
		if (direction == UP_COLLISION)
			position.y = col_2->rect.y - colliderSize.y + 1;
		
		else if (direction == DOWN_COLLISION)
			position.y = col_2->rect.y + col_2->rect.h;
		
		else if (direction == RIGHT_COLLISION)
			position.x = col_2->rect.x + col_2->rect.w;
		
		else if (direction == LEFT_COLLISION)
			position.x = col_2->rect.x - colliderSize.x;
	}

	if (col_2->type == COLLIDER_ATTACK || col_2->type == COLLIDER_DEATH) {
		App->entity->player->score_points += 50;
		dead = true;
		collider->to_delete = true;
		int num = App->entity->entities.find(this);
		RELEASE(App->entity->entities.At(num)->data);
		App->entity->entities.del(App->entity->entities.At(num));
	}
	
}

bool j1Skeleton::Load(pugi::xml_node& data)
{
	return true;
}

bool j1Skeleton::Save(pugi::xml_node& data) const
{
	pugi::xml_node pos = data.append_child("position");

	pos.append_attribute("x") = position.x;
	pos.append_attribute("y") = position.y;

	return true;
}

void j1Skeleton::LoadSkeletonProperties()
{
	pugi::xml_document config_file;
	config_file.load_file("config.xml");
	pugi::xml_node config;
	config = config_file.child("config");
	pugi::xml_node skeleton;
	skeleton = config.child("skeleton");

	speed = skeleton.attribute("speed").as_int();

	// Copying the values of the collider
	margin.x = skeleton.child("margin").attribute("x").as_int();
	margin.y = skeleton.child("margin").attribute("y").as_int();
	colliderSize.x = skeleton.child("colliderSize").attribute("w").as_int();
	colliderSize.y = skeleton.child("colliderSize").attribute("h").as_int();
}

void j1Skeleton::Move(p2DynArray<iPoint>& path, float dt)
{
	direction = App->path->CheckDirectionGround(path);

	if (direction == Movement::DOWN)
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
