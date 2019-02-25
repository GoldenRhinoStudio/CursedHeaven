#ifndef __j1COIN_H__
#define __j1COIN_H__

#include "j1Entity.h"
#include "p2Point.h"
#include "j1Collisions.h"

class j1Coin : public j1Entity
{
public:
	
	j1Coin(int x, int y, ENTITY_TYPES type);
	
	virtual ~j1Coin();

	bool Start();

	bool Update(float dt, bool do_logic);

	bool CleanUp();

	void OnCollision(Collider* c1, Collider* c2);

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	uint coin_fx;

	Animation idle;
};

#endif //__j1COIN_H__