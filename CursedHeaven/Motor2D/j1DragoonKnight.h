#ifndef __j1DRAGOONKNIGHT_H__
#define __j1DRAGOONKNIGHT_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2Point.h"
#include "p2Animation.h"
#include "j1Player.h"

class j1Timer;
struct SDL_Texture;
enum ENTITY_TYPES;

class j1DragoonKnight : public j1Player
{

public:
	j1DragoonKnight(int x, int y, ENTITY_TYPES type);

	// Destructor
	virtual ~j1DragoonKnight();

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

	Animation attack_up;
	Animation attack_down;
	Animation attack_diagonal_up_right;
	Animation attack_diagonal_up_left;
	Animation attack_diagonal_down_right;
	Animation attack_diagonal_down_left;
	Animation attack_lateral_right;
	Animation attack_lateral_left;

	Animation godmode;
	Animation death;

	// Abilities
	j1Timer cooldown_Q;
	uint lastTime_Q = 0;
	fPoint lastPosition;
	uint dashSpeed = 0;

	j1Timer cooldown_E;
	j1Timer cooldown_Rage;
	uint lastTime_E = 0;
	uint lastTime_Rage = 0;
	uint rageDamage = 0;

	// Sounds

};

#endif // __j1DRAGOONKNIGHT_H__