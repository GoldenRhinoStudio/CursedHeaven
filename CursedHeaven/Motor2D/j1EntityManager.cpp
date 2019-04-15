#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1EntityManager.h"
#include "j1Entity.h"
#include "j1Scene1.h"
#include "j1Player.h"
#include "Brofiler/Brofiler.h"

j1EntityManager::j1EntityManager()
{
	name.assign("entityManager");
}

j1EntityManager::~j1EntityManager() {}

bool j1EntityManager::Start()
{

	for (std::list<j1Entity*>::iterator item = entities.begin(); item != entities.end(); ++item)
	{
		(*item)->Start();
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

	for (std::list<j1Entity*>::iterator item = entities.begin(); item != entities.end(); ++item)
	{
		(*item)->Update(dt, do_logic);
	}

	if (do_logic) {
		accumulatedTime = 0.0f;
		do_logic = false;
	}

	App->render->reOrder();
	App->render->OrderBlit(App->render->OrderToRender);

	return true;
}

bool j1EntityManager::PostUpdate()
{
	BROFILER_CATEGORY("EntityManagerPostUpdate", Profiler::Color::Yellow)

	for (std::list<j1Entity*>::iterator item = entities.begin(); item != entities.end(); ++item)
	{
		(*item)->PostUpdate();
	}

	return true;
}

bool j1EntityManager::CleanUp()
{
	LOG("Freeing all enemies");

	
	for (std::list<j1Entity*>::iterator item = entities.begin(); item != entities.end(); ++item)
	{
		(*item)->CleanUp();
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
		if (ret != nullptr) 
			entities.push_back(ret); 
		break;
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

			entities.push_back(entity);
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
	
	for (std::list<j1Entity*>::iterator item = entities.begin(); item != entities.end(); ++item)
	{

	}
}

void j1EntityManager::CreatePlayer() 
{
	hook = (j1Hook*)CreateEntity(HOOK);
	player = (j1Player*)CreateEntity(PLAYER);
}

void j1EntityManager::OnCollision(Collider* c1, Collider* c2)
{
	for (std::list<j1Entity*>::iterator item = entities.begin(); item != entities.end(); ++item)
	{
		if ((*item)->collider == c1) 
		{ 
			(*item)->OnCollision(c1, c2); 	
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

	return true;
}

bool j1EntityManager::Save(pugi::xml_node& data) const
{
	player->Save(data.append_child("player"));


	return true;
}