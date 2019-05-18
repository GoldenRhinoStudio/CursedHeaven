#ifndef __ITEMS_H__
#define __ITEMS_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2Point.h"
#include "p2Animation.h"
#include "j1Entity.h"

struct SDL_Texture;

class Items : public j1Entity
{
public:
	Items(int x, int y, ENTITY_TYPES type, ITEM_TYPES itype);
	virtual ~Items();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt, bool do_logic);

	bool DrawOrder(float dt);

	// Called before quitting
	bool CleanUp();

	bool PostUpdate();

	// Called to check collisions
	void OnCollision(Collider* col_1, Collider* col_2);

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	void LoadProperties();

	const char * ToString(ITEM_TYPES item);

public:
	fPoint initialPosition;
	iPoint colliderSize;
	int value;

private:
	Animation item;
	ITEM_TYPES itype;
};
#endif // __j1SLIME_H__