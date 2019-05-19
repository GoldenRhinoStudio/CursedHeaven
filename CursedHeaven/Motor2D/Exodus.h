
#ifndef __EXODUS_H__
#define __EXODUS_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2Point.h"
#include "p2Animation.h"
#include "j1Pathfinding.h"
#include "j1Entity.h"
#include "j1Timer.h"

#include <vector>

#define EX_DETECTION_RANGE 20

struct SDL_Texture;
class Exodus : public j1Entity
{
public:
	Exodus(int x, int y, ENTITY_TYPES type);
	virtual ~Exodus();

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
	void Move(const std::vector<iPoint>* path, float dt);

public:
	fPoint initialPosition;

	fPoint LeftHandPos;
	fPoint RightHandPos;

	iPoint margin;
	iPoint colliderSize;

	Animation idle;

	j1Timer shotTimer;

	int state = 0;
	int attack = 0;
	int damage = 0;
	int shotTime = 0;
	bool dead = false;
	bool receivedBasicDamage = false;
	bool receivedAbilityDamage = false;
	bool start_fight = false;
};
#endif // __j1MINDFLYER_H__