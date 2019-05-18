#ifndef __j1TURRET_H__
#define __j1TURRET_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2Point.h"
#include "p2Animation.h"
#include "j1Entity.h"
#include "j1Timer.h"

#include <vector>

#define ATTACK_RANGE_TURRET 10

struct SDL_Texture;
//struct Collider;

class j1Turret : public j1Entity
{
public:
	j1Turret(int x, int y, ENTITY_TYPES type);

	// Destructor
	virtual ~j1Turret();

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

public:
	iPoint margin;
	iPoint colliderSize;

	bool target_found = false;
	bool dead = false;
	bool receivedBasicDamage = false;
	bool receivedAbilityDamage = false;

private:
	Animation idle;

	j1Timer shotTimer;
	uint lastTime_Shot = 0;
	uint cooldown_Shot = 0;

	int score = 0;

};
#endif // __j1TURRET_H__