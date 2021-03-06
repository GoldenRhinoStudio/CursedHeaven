#ifndef __J1ENTITYMANAGER_H__
#define __J1ENTITYMANAGER_H__

#include "j1Module.h"
#include "j1UserInterfaceElement.h"

#include <list>

#define MAX_ENTITIES 1000

class j1Entity;
class j1DragoonKnight;
class j1BlackMage;
class j1Rogue;
class j1Tank;
class j1Player;
class j1Judge;
class j1Seller;
class Exodus;

struct SDL_Texture;

enum DIRECTION {
	NONE_,
	UP_,
	DOWN_,
	RIGHT_,
	LEFT_,
	UP_RIGHT_,
	UP_LEFT_,
	DOWN_RIGHT_,
	DOWN_LEFT_	
};

enum ENTITY_TYPES
{
	PLAYER,
	COIN,
	LIFE,
	ITEM,
	SLIME,
	TURRET,
	FIRE,
	MINDFLYER,
	EXODUS,
	JUDGE,
	OLDMAN,
	SELLER,
	UNKNOWN
};

enum PLAYER_TYPES
{
	KNIGHT,
	MAGE,
	NO_PLAYER
};

struct EntityInfo
{
	ENTITY_TYPES type = ENTITY_TYPES::UNKNOWN;
	iPoint position;
};

class j1EntityManager : public j1Module
{
public:
	// Constructor
	j1EntityManager();

	// Destructor
	~j1EntityManager();

	bool Awake(pugi::xml_node&);
	bool Start();

	// Called every frame
	bool PreUpdate();
	bool Update(float dt);
	bool DrawEntityOrder(float dt);
	bool PostUpdate();

	bool CleanUp();
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	j1Entity* CreateEntity(ENTITY_TYPES type, int x = 0, int y = 0);
	
	void OnCollision(Collider* c1, Collider* c2);
	void CreatePlayer1();
	void CreatePlayer2();
	void AddEnemy(int x, int y, ENTITY_TYPES type);
	void DestroyEntities();
	void AddItem(int x, int y, ENTITY_TYPES itype);

private:
	void SpawnEnemy(const EntityInfo& info);

public:
	
	std::list<j1Entity*> entities;

	// Pointers to diferent playable classes
	j1DragoonKnight*	knight = nullptr;
	j1BlackMage*		mage = nullptr;
	j1Tank*				tank = nullptr;
	j1Rogue*			rogue = nullptr;

	j1Player*			currentPlayer = nullptr;
	PLAYER_TYPES player_type = NO_PLAYER;

	j1Judge*			judge = nullptr;
	j1Seller*			seller = nullptr;
	Exodus*				exo = nullptr;

	int mindflyer_Damage = 0;
	int slime_Damage = 0;
	int fire_Damage = 0;
	int turret_Damage = 0;

	int mf_lifePoints = 0;
	bool loadingMf = false;
	int exo_lifePoints = 0;
	bool loadingExo = false;

private:
	int entity_logic = 0;
	EntityInfo			queue[MAX_ENTITIES];
	bool				do_logic = false;
	float				accumulatedTime = 0.0f;
	float				updateMsCycle = 0.0f;
	fPoint				playerSpawnPos1;
	fPoint				playerSpawnPos2;
};

#endif // __J1ENTITYMANAGER_H__