#ifndef __j1BLACKMAGE_H__
#define __j1BLACKMAGE_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2Point.h"
#include "p2Animation.h"
#include "j1Player.h"

class j1Timer;
struct SDL_Texture;
enum ENTITY_TYPES;

class j1BlackMage : public j1Player
{

public:
	j1BlackMage(int x, int y, ENTITY_TYPES type);

	// Destructor
	virtual ~j1BlackMage();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt, bool do_logic);
	bool PostUpdate();

	void LoadPlayerProperties();

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	// Called before quitting
	bool CleanUp();

public:

	// Animations of the mage
	Animation attack_up;
	Animation attack_down;
	Animation attack_diagonal_up;
	Animation attack_diagonal_down;
	Animation attack_lateral;

	// Abilities
	j1Timer cooldown_Q;
	uint cooldownTime_Q = 0;
	uint lastTime_Q = 0;
	uint fireDamage = 0;

	j1Timer cooldown_E;
	uint cooldownTime_E = 0;
	j1Timer cooldown_Speed;
	uint cooldownTime_Speed = 0;
	uint lastTime_E = 0;
	uint lastTime_Speed = 0;

	// Sounds

};

#endif // __j1BLACKMAGE_H__