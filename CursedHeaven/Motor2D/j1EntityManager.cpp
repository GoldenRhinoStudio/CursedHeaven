#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1EntityManager.h"
#include "j1Entity.h"
#include "j1Scene1.h"
#include "j1DragoonKnight.h"
#include "j1BlackMage.h"
#include "j1Rogue.h"
#include "j1Tank.h"
#include "j1Judge.h"
#include "j1Seller.h"
#include "j1Slime.h"
#include "j1Fire.h"
#include "j1Turret.h"
#include "j1MindFlyer.h"
#include "j1Map.h"
#include "Items.h"
#include "Exodus.h"

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
	playerSpawnPos1.x = config.child("position1").attribute("x").as_float();
	playerSpawnPos1.y = config.child("position1").attribute("y").as_float();
	playerSpawnPos2.x = config.child("position2").attribute("x").as_float();
	playerSpawnPos2.y = config.child("position2").attribute("y").as_float();

	return true;
}

bool j1EntityManager::Update(float dt)
{
	BROFILER_CATEGORY("EntityManagerUpdate", Profiler::Color::LightSeaGreen)

	accumulatedTime += dt;
	if (accumulatedTime >= updateMsCycle && !do_logic) {
		do_logic = true;
		entity_logic = 0;
	}

	int i = 0;
	for (std::list<j1Entity*>::iterator item = entities.begin(); item != entities.end(); ++item)
	{
		if (do_logic){
			if (entity_logic == i) {
				(*item)->Update(dt, true);
			}
			else
				(*item)->Update(dt, false);
			i++;
		}else
			(*item)->Update(dt, do_logic);
	}

	entity_logic++;

	if (do_logic && entity_logic >= entities.size()) {
		accumulatedTime = 0.0f;
		do_logic = false;
	}

	return true;
}

bool j1EntityManager::DrawEntityOrder(float dt)
{
	BROFILER_CATEGORY("EntityManagerUpdate", Profiler::Color::LightSeaGreen)

	
	for (std::list<j1Entity*>::iterator item = entities.begin(); item != entities.end(); ++item)
	{
		(*item)->DrawOrder(dt);
	}

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
	LOG("Freeing all entities");

	for (std::list<j1Entity*>::iterator item = entities.begin(); item != entities.end(); ++item)
	{
		(*item)->CleanUp();
	}

	entities.clear();

	knight = nullptr;
	mage = nullptr;
	tank = nullptr;
	rogue = nullptr;

	return true;
}

j1Entity* j1EntityManager::CreateEntity(ENTITY_TYPES type, int x, int y)
{
	j1Entity* ret = nullptr;
	switch (type)
	{
	case PLAYER: 
		if (player_type == KNIGHT) ret = new j1DragoonKnight(x, y, type);
		else if (player_type == MAGE) ret = new j1BlackMage(x, y, type);
		/*else if (player_type == TANK) ret = new j1Tank(x, y, type);
		else if (player_type == ROGUE) ret = new j1Rogue(x, y, type);*/

		if (ret != nullptr) 
			entities.push_back(ret); 
		break;

	case JUDGE:
		ret = new j1Judge(x, y, type);

	case SELLER:
		ret = new j1Seller(x, y, type);

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
			queue[i].position.x = App->map->MapToWorld(x, y).x;
			queue[i].position.y = App->map->MapToWorld(x, y).y;
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
			if (queue[i].type == SLIME)
				entity = new j1Slime(info.position.x, info.position.y, info.type);
			else if (queue[i].type == MINDFLYER)
				entity = new j1MindFlyer(info.position.x, info.position.y, info.type);
			else if (queue[i].type == TURRET)
				entity = new j1Turret(info.position.x, info.position.y, info.type);
			else if (queue[i].type == FIRE)
				entity = new j1Fire(info.position.x, info.position.y, info.type);
			else if (queue[i].type == EXODUS) {
				entity = new Exodus(info.position.x, info.position.y, info.type);
				exodus = (Exodus*)entity;
			}

			entities.push_back(entity);
			entity->Start();
			break;
		}
	}
}

void j1EntityManager::AddItem(int x, int y, DROP_TYPES itype)
{
	j1Entity* ret = nullptr;

	ret = new Items(x, y, ENTITY_TYPES::ITEM, itype);

	if (ret != nullptr)
		entities.push_back(ret);

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

void j1EntityManager::CreatePlayer1()
{
	if (player_type == KNIGHT) knight = (j1DragoonKnight*)CreateEntity(PLAYER, playerSpawnPos1.x, playerSpawnPos1.y);
	else if (player_type == MAGE) mage = (j1BlackMage*)CreateEntity(PLAYER, playerSpawnPos1.x, playerSpawnPos1.y);

	if (knight != nullptr) currentPlayer = knight;
	else if (mage != nullptr) currentPlayer = mage;

	currentPlayer->invulCounter.Start();
	currentPlayer->potionTime.Start();
}

void j1EntityManager::CreatePlayer2()
{
	if (player_type == KNIGHT) knight = (j1DragoonKnight*)CreateEntity(PLAYER, playerSpawnPos2.x, playerSpawnPos2.y);
	else if (player_type == MAGE) mage = (j1BlackMage*)CreateEntity(PLAYER, playerSpawnPos2.x, playerSpawnPos2.y);

	if (knight != nullptr) currentPlayer = knight;
	else if (mage != nullptr) currentPlayer = mage;

	currentPlayer->invulCounter.Start();
	currentPlayer->potionTime.Start();
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

	for (pugi::xml_node slime = data.child("slime").child("position"); slime; slime = slime.next_sibling())
	{
		iPoint pos = { slime.attribute("x").as_int(), slime.attribute("y").as_int() };
		AddEnemy(pos.x, pos.y, SLIME);
	}

	for (pugi::xml_node fire = data.child("fire").child("position"); fire; fire = fire.next_sibling())
	{
		iPoint pos = { fire.attribute("x").as_int(), fire.attribute("y").as_int() };
		AddEnemy(pos.x, pos.y, FIRE);
	}

	for (pugi::xml_node turret = data.child("turret").child("position"); turret; turret = turret.next_sibling())
	{
		iPoint pos = { turret.attribute("x").as_int(), turret.attribute("y").as_int() };
		AddEnemy(pos.x, pos.y, TURRET);
	}

	pugi::xml_node mindflyer = data.child("mindflyer");
	pugi::xml_node exodus = data.child("exodus");

	for (std::list<j1Entity*>::iterator item = entities.begin(); item != entities.end(); ++item)
	{
		if (item._Ptr->_Myval->type == MINDFLYER)
			item._Ptr->_Myval->Load(mindflyer);

		else if (item._Ptr->_Myval->type == EXODUS)
			item._Ptr->_Myval->Load(exodus);
	}

	pugi::xml_node type = data.child("playerType");

	player_type = (PLAYER_TYPES)type.attribute("type").as_uint();
	App->gamePaused = data.child("gamePaused").attribute("value").as_bool();

	CreatePlayer1();
	Start();

	if (knight != nullptr) knight->Load(data);
	else if (mage != nullptr) mage->Load(data);

	return true;
}

bool j1EntityManager::Save(pugi::xml_node& data) const
{
	if (player_type == KNIGHT) knight->Save(data.append_child("player"));
	else if (player_type == MAGE) mage->Save(data.append_child("player"));

	pugi::xml_node p_type = data.append_child("playerType"); 
	p_type.append_attribute("type") = (uint)player_type;

	pugi::xml_node pause = data.append_child("gamePaused");
	pause.append_attribute("value") = App->gamePaused;

	pugi::xml_node slime = data.append_child("slime");
	pugi::xml_node fire = data.append_child("fire");
	pugi::xml_node turret = data.append_child("turret");
	pugi::xml_node mindflyer = data.append_child("mindflyer");
	pugi::xml_node exodus = data.append_child("exodus");

	for (std::list<j1Entity*>::iterator item = App->entity->entities.begin(); item != entities.end(); ++item)
	{
		if (item._Ptr->_Myval->type == SLIME)
			item._Ptr->_Myval->Save(slime);

		if (item._Ptr->_Myval->type == FIRE)
			item._Ptr->_Myval->Save(fire);

		else if (item._Ptr->_Myval->type == TURRET)
			item._Ptr->_Myval->Save(turret);

		else if (item._Ptr->_Myval->type == MINDFLYER)
			item._Ptr->_Myval->Save(mindflyer);

		else if (item._Ptr->_Myval->type == EXODUS)
			item._Ptr->_Myval->Save(exodus);
	}		

	return true;
}