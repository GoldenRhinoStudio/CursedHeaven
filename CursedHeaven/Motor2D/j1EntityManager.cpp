#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1EntityManager.h"
#include "j1Entity.h"
#include "j1Scene1.h"
#include "j1Scene2.h"
#include "j1Harpy.h"
#include "j1Player.h"
#include "j1Hook.h"
#include "j1Skeleton.h"
#include "j1Coin.h"
#include "Brofiler/Brofiler.h"

j1EntityManager::j1EntityManager()
{
	name.create("entityManager");
}

j1EntityManager::~j1EntityManager() {}

bool j1EntityManager::Start()
{
	for (p2List_item<j1Entity*>* iterator = entities.start; iterator != nullptr; iterator = iterator->next)
	{ 
		iterator->data->Start();
	}

	return true;
}

bool j1EntityManager::PreUpdate()
{
	BROFILER_CATEGORY("EntityManagerPreUpdate", Profiler::Color::Orange)

	for (uint i = 0; i < MAX_ENTITIES; ++i)
	{
		if (queue[i].type != ENTITY_TYPES::UNKNOWN)
		{
			SpawnEnemy(queue[i]);
			queue[i].type = ENTITY_TYPES::UNKNOWN;		
		}
	}

	return true;
}

bool j1EntityManager::Awake(pugi::xml_node& config)
{
	LOG("Awaking Entity manager");
	updateMsCycle = config.attribute("updateMsCycle").as_float();

	return true;
}

bool j1EntityManager::Update(float dt)
{
	BROFILER_CATEGORY("EntityManagerUpdate", Profiler::Color::LightSeaGreen)

	accumulatedTime += dt;
	if (accumulatedTime >= updateMsCycle)
		do_logic = true;

	for (p2List_item<j1Entity*>* iterator = entities.start; iterator != nullptr; iterator = iterator->next)
	{
		iterator->data->Update(dt, do_logic);
	}

	if (do_logic) {
		accumulatedTime = 0.0f;
		do_logic = false;
	}

	return true;
}

bool j1EntityManager::PostUpdate()
{
	BROFILER_CATEGORY("EntityManagerPostUpdate", Profiler::Color::Yellow)

	for (p2List_item<j1Entity*>* iterator = entities.start; iterator != nullptr; iterator = iterator->next)
	{
		iterator->data->PostUpdate();
	}

	return true;
}

bool j1EntityManager::CleanUp()
{
	LOG("Freeing all enemies");

	
	for (p2List_item<j1Entity*>* iterator = entities.start; iterator != nullptr; iterator = iterator->next) 
	{
		iterator->data->CleanUp();
	}

	entities.clear();

	player = nullptr;
	hook = nullptr;

	return true;
}

j1Entity* j1EntityManager::CreateEntity(ENTITY_TYPES type, int x, int y)
{
	j1Entity* ret = nullptr;
	switch (type)
	{
	case PLAYER: 
		ret = new j1Player(x, y, type);
	if (ret != nullptr) entities.add(ret); break;

	case HOOK:
		ret = new j1Hook(x, y, type);
		if (ret != nullptr) entities.add(ret); break; 
	}
	return ret;
}

void j1EntityManager::AddEnemy(int x, int y, ENTITY_TYPES type)
{
	for (uint i = 0; i < MAX_ENTITIES; ++i)
	{
		if (queue[i].type == ENTITY_TYPES::UNKNOWN)
		{
			queue[i].type = type;
			queue[i].position.x = x;
			queue[i].position.y = y;
			break;
		}
	}
}

void j1EntityManager::SpawnEnemy(const EntityInfo& info)
{
	for (uint i = 0; i < MAX_ENTITIES; ++i)
	{
		if (queue[i].type != ENTITY_TYPES::UNKNOWN)
		{
			j1Entity* entity;
			if (queue[i].type == HARPY) 
				entity = new j1Harpy(info.position.x, info.position.y, info.type); 

			else if (queue[i].type == SKELETON)
				entity = new j1Skeleton(info.position.x, info.position.y, info.type);

			else if (queue[i].type == COIN)
				entity = new j1Coin(info.position.x, info.position.y, info.type);

			entities.add(entity);
			entity->Start();
			break;
		}
	}
}

void j1EntityManager::DestroyEntities()
{
	for (int i = 0; i < MAX_ENTITIES; i++) 
	{ 
		queue[i].type = ENTITY_TYPES::UNKNOWN; 
	}	
	
	for (p2List_item<j1Entity*>* iterator = entities.start; iterator; iterator = iterator->next) {
		if (iterator->data->type != ENTITY_TYPES::PLAYER && iterator->data->type != ENTITY_TYPES::HOOK)
		{
			iterator->data->CleanUp();
			int num = entities.find(iterator->data);
			RELEASE(entities.At(num)->data);
			entities.del(entities.At(num));
		}
	}
}

void j1EntityManager::CreatePlayer() 
{
	hook = (j1Hook*)CreateEntity(HOOK);
	player = (j1Player*)CreateEntity(PLAYER);
}

void j1EntityManager::OnCollision(Collider* c1, Collider* c2)
{
	for (p2List_item<j1Entity*>* iterator = entities.start; iterator != nullptr; iterator = iterator->next)
	{
		if (iterator->data->collider == c1) 
		{ 
			iterator->data->OnCollision(c1, c2); 	
			break; 
		}
	}
}

bool j1EntityManager::Load(pugi::xml_node& data)
{
	DestroyEntities();

	if (player != nullptr)
	{
		player->Load(data);
	}

	for (pugi::xml_node harpy = data.child("harpy").child("position"); harpy; harpy = harpy.next_sibling()) 
	{
		iPoint pos = { harpy.attribute("x").as_int(), harpy.attribute("y").as_int() };
		AddEnemy(pos.x, pos.y, HARPY);
	}

	for (pugi::xml_node skeleton = data.child("skeleton").child("position"); skeleton; skeleton = skeleton.next_sibling())
	{
		iPoint pos = { skeleton.attribute("x").as_int(), skeleton.attribute("y").as_int() };
		AddEnemy(pos.x, pos.y, SKELETON);
	}

	for (pugi::xml_node coin = data.child("coin").child("position"); coin; coin = coin.next_sibling())
	{
		iPoint pos = { coin.attribute("x").as_int(), coin.attribute("y").as_int() };
		AddEnemy(pos.x, pos.y, COIN);
	}

	return true;
}

bool j1EntityManager::Save(pugi::xml_node& data) const
{
	player->Save(data.append_child("player"));

	pugi::xml_node harpy = data.append_child("harpy");
	pugi::xml_node skeleton = data.append_child("skeleton");
	pugi::xml_node coin = data.append_child("coin");

	p2List_item<j1Entity*>* iterator;
	for (iterator = entities.start; iterator; iterator = iterator->next)
	{
		if (iterator->data->type == HARPY) 
			iterator->data->Save(harpy);
		
		else if (iterator->data->type == SKELETON) 
			iterator->data->Save(skeleton);
		
		else if (iterator->data->type == COIN) 
			iterator->data->Save(coin);
	}

	for (int i = 0; i < MAX_ENTITIES; ++i)
	{
		if (queue[i].type != ENTITY_TYPES::UNKNOWN) 
		{
			if (queue[i].type == HARPY)
			{
				pugi::xml_node harpy_pos = harpy.append_child("position");
				harpy_pos.append_attribute("x") = queue[i].position.x;
				harpy_pos.append_attribute("y") = queue[i].position.y;
			}
			else if (queue[i].type == SKELETON) 
			{
				pugi::xml_node skeleton_pos = skeleton.append_child("position");
				skeleton_pos.append_attribute("x") = queue[i].position.x;
				skeleton_pos.append_attribute("y") = queue[i].position.y;
			}
			else if (queue[i].type == COIN) 
			{
				pugi::xml_node coin_pos = coin.append_child("position");
				coin_pos.append_attribute("x") = queue[i].position.x;
				coin_pos.append_attribute("y") = queue[i].position.y;
			}
		}
	}



	return true;
}