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

	// Animations of the player	
	Animation idle_diagonal_up;
	Animation idle_diagonal_down;
	Animation idle_lateral;
	Animation idle_down;
	Animation idle_up;

	Animation diagonal_up;
	Animation diagonal_down;
	Animation lateral;
	Animation up;
	Animation down;

	Animation godmode;
	Animation death;

	// Attacks
	j1Timer cooldown_Q;
	uint lastTime_Q = 0;
	j1Timer cooldown_E;
	uint lastTime_E = 0;

	// Sounds

};

#endif // __j1BLACKMAGE_H__