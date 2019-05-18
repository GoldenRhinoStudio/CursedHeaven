#ifndef __j1FIRE_H__
#define __j1FIRE_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2Point.h"
#include "p2Animation.h"
#include "j1Pathfinding.h"
#include "j1Entity.h"

#include <vector>

#define ATTACK_RANGE_FIRE 0

struct SDL_Texture;
//struct Collider;

class j1Fire : public j1Entity
{
public:
	j1Fire(int x, int y, ENTITY_TYPES type);

	// Destructor
	virtual ~j1Fire();

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

	iPoint margin;
	iPoint colliderSize;

	float speed;

	Movement direction;

	std::vector<iPoint>* path;
	int node = 0;

	bool target_found = false;
	bool dead = false;
	bool receivedBasicDamage = false;
	bool receivedAbilityDamage = false;

private:
	Animation idle;

	int score = 0;

};
#endif // __j1SLIME_H__