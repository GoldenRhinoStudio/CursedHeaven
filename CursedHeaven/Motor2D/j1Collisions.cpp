#include "j1Collisions.h"
#include "p2Defs.h"
#include "j1Module.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Player.h"
#include "p2Log.h"
#include "j1Map.h"

#include "Brofiler/Brofiler.h"
#include "SDL\include\SDL.h"

j1Collisions::j1Collisions() : j1Module()
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		colliders[i] = nullptr;
	}

	name.assign("collisions");

	matrix[COLLIDER_NONE][COLLIDER_NONE] = false;
	matrix[COLLIDER_NONE][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_NONE][COLLIDER_ENEMY_SHOT] = false;
	matrix[COLLIDER_NONE][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_NONE][COLLIDER_ATTACK] = false;
	matrix[COLLIDER_NONE][COLLIDER_DROP] = false;
	matrix[COLLIDER_NONE][COLLIDER_ABILITY] = false;
	matrix[COLLIDER_NONE][COLLIDER_ITEM] = true;

	matrix[COLLIDER_PLAYER][COLLIDER_NONE] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_ENEMY_SHOT] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_ATTACK] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_DROP] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_ABILITY] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_ITEM] = true;

	matrix[COLLIDER_ENEMY_SHOT][COLLIDER_NONE] = false;
	matrix[COLLIDER_ENEMY_SHOT][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_ENEMY_SHOT][COLLIDER_ENEMY_SHOT] = false;
	matrix[COLLIDER_ENEMY_SHOT][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_ENEMY_SHOT][COLLIDER_ATTACK] = false;
	matrix[COLLIDER_ENEMY_SHOT][COLLIDER_DROP] = false;
	matrix[COLLIDER_ENEMY_SHOT][COLLIDER_ABILITY] = false;
	matrix[COLLIDER_ENEMY_SHOT][COLLIDER_ITEM] = false;

	matrix[COLLIDER_ENEMY][COLLIDER_NONE] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_ENEMY_SHOT] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_ATTACK] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_DROP] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_ABILITY] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_ITEM] = false;

	matrix[COLLIDER_ATTACK][COLLIDER_ATTACK] = false;
	matrix[COLLIDER_ATTACK][COLLIDER_NONE] = false;
	matrix[COLLIDER_ATTACK][COLLIDER_ENEMY_SHOT] = false;
	matrix[COLLIDER_ATTACK][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_ATTACK][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_ATTACK][COLLIDER_DROP] = false;
	matrix[COLLIDER_ATTACK][COLLIDER_ABILITY] = false;
	matrix[COLLIDER_ATTACK][COLLIDER_ITEM] = false;

	matrix[COLLIDER_DROP][COLLIDER_ATTACK] = false;
	matrix[COLLIDER_DROP][COLLIDER_NONE] = false;
	matrix[COLLIDER_DROP][COLLIDER_ENEMY_SHOT] = false;
	matrix[COLLIDER_DROP][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_DROP][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_DROP][COLLIDER_DROP] = false;
	matrix[COLLIDER_DROP][COLLIDER_ABILITY] = false;
	matrix[COLLIDER_DROP][COLLIDER_ITEM] = false;

	matrix[COLLIDER_ABILITY][COLLIDER_ATTACK] = false;
	matrix[COLLIDER_ABILITY][COLLIDER_NONE] = false;
	matrix[COLLIDER_ABILITY][COLLIDER_ENEMY_SHOT] = false;
	matrix[COLLIDER_ABILITY][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_ABILITY][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_ABILITY][COLLIDER_DROP] = false;
	matrix[COLLIDER_ABILITY][COLLIDER_ABILITY] = false;
	matrix[COLLIDER_ABILITY][COLLIDER_ITEM] = false;

	matrix[COLLIDER_ITEM][COLLIDER_ATTACK] = false;
	matrix[COLLIDER_ITEM][COLLIDER_NONE] = true;
	matrix[COLLIDER_ITEM][COLLIDER_ENEMY_SHOT] = false;
	matrix[COLLIDER_ITEM][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_ITEM][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_ITEM][COLLIDER_DROP] = false;
	matrix[COLLIDER_ITEM][COLLIDER_ABILITY] = false;
	matrix[COLLIDER_ITEM][COLLIDER_ITEM] = false;
}

j1Collisions::~j1Collisions() {}

bool j1Collisions::PreUpdate()
{
	BROFILER_CATEGORY("CollisionPreUpdate", Profiler::Color::Orange)

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr) 
		{
			if (colliders[i]->to_delete) 
			{
				delete colliders[i];
				colliders[i] = nullptr;
			}
		}
	}

	return true;
}

bool j1Collisions::Update(float dt) 
{
	BROFILER_CATEGORY("CollisionUpdate", Profiler::Color::LightSeaGreen)

	Collider* collider1;
	Collider* collider2;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr) continue;

		if (colliders[i]->type == COLLIDER_PLAYER || colliders[i]->type == COLLIDER_NONE || colliders[i]->type == COLLIDER_ENEMY || colliders[i]->type == COLLIDER_ITEM ||
			colliders[i]->type == COLLIDER_DROP || colliders[i]->type == COLLIDER_ABILITY || colliders[i]->type == COLLIDER_ENEMY_SHOT || colliders[i]->type ==	COLLIDER_ATTACK)
		{
			collider1 = colliders[i];

			for (uint j = 0; j < MAX_COLLIDERS; ++j)
			{
				if (colliders[j] == nullptr || i == j) continue;

				collider2 = colliders[j];

				if (collider1->CheckCollision(collider2->rect) == true)
				{
					if (matrix[collider1->type][collider2->type] && collider1->callback)
					{
						collider1->callback->OnCollision(collider1, collider2);
					}
				}
			}
		}
	}

	DrawColliders();

	return true;
}

bool j1Collisions::CleanUp()
{
	LOG("Freeing all colliders");

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return true;
}

void j1Collisions::DrawColliders() 
{
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)	
		debug = !debug;

	if (debug == false)
		return;

	Uint8 alpha = 80;
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
			continue;

		switch (colliders[i]->type)
		{
		case COLLIDER_NONE:			//White
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 255, alpha);
			break;
		case COLLIDER_PLAYER:		//Green															
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 0, alpha);
			break;
		case COLLIDER_ENEMY_SHOT:	//Dark green
			App->render->DrawQuad(colliders[i]->rect, 0, 71, 49, alpha);
			break;
		case COLLIDER_ENEMY:		//Red
			App->render->DrawQuad(colliders[i]->rect, 255, 0, 0, alpha);
			break;
		case COLLIDER_ATTACK:		//Cyan
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 255, alpha);
			break;
		case COLLIDER_DROP:			//Yellow
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 0, alpha);
			break;
		case COLLIDER_ABILITY:		//Orange
			App->render->DrawQuad(colliders[i]->rect, 253, 106, 2, alpha);
			break;
		case COLLIDER_ITEM:			//Black
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 0, alpha);
			break;
		}
	}
}

Collider* j1Collisions::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback)
{
	Collider* ret = nullptr;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
		{
			ret = colliders[i] = new Collider(rect, type, callback);
			break;
		}
	}
	return ret;
}

bool Collider::CheckCollision(const SDL_Rect& r) const
{
	return (rect.x < r.x + r.w &&
		rect.x + rect.w > r.x &&
		rect.y < r.y + r.h &&
		rect.h + rect.y > r.y);
}