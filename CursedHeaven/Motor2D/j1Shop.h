#ifndef __J1SHOP_H__
#define __J1SHOP_H__

#include "j1Module.h"
#include "j1Entity.h"
#include "j1Timer.h"
#include <list>

struct SDL_Texture;
struct Collider;
class j1Label;

enum ITEM_TYPE
{
	BOOTS = 0,
	SWORD,
	HEART,
	ARMOUR,
	HOURGLASS,
	BOOK,
	POTION,
	NO_ITEM
};

class j1Item : public j1Entity
{
public:
	// Constructor
	j1Item(int x, int y, ITEM_TYPE type);

	// Destructor
	~j1Item();

	bool Start();

	// Called every frame
	bool Update(float dt, bool do_logic);
	void OnCollision(Collider* c1, Collider* c2);

	bool CleanUp();

public:
	uint prize;
	uint level = 0;
	ITEM_TYPE type = NO_ITEM;

private:
	SDL_Rect image;
	SDL_Rect labelBg;

	j1Label* description;
};

// ---------------------------------------------------------------

class j1Shop : public j1Module
{
public:
	// Constructor
	j1Shop();

	// Destructor
	~j1Shop();

	bool Awake(pugi::xml_node&);
	bool Start();

	// Called every frame
	bool Update(float dt);
	bool CleanUp();

	void OnCollision(Collider* c1, Collider* c2);
	j1Item* j1Shop::CreateItem(ITEM_TYPE type, int x, int y);
	void PlaceShop();


public:
	std::list<j1Item*> items;
	std::list<j1Label*> itemLabels;

	// Prizes
	uint boots_prize1, boots_prize2, boots_prize3;
	uint sword_prize1, sword_prize2, sword_prize3;
	uint heart_prize1, heart_prize2, heart_prize3;
	uint armour_prize1, armour_prize2, armour_prize3;
	uint hourglass_prize1, hourglass_prize2, hourglass_prize3;
	uint book_prize1, book_prize2, book_prize3;
	uint potion_prize;

	// Items
	uint potions = 0;
	uint bootsLevel = 0;
	uint swordLevel = 0;
	uint heartLevel = 0;
	uint armourLevel = 0;
	uint hourglassLevel = 0;
	uint bookLevel = 0;

	uint potionHealing = 0;
};

#endif // __J1SHOP_H__