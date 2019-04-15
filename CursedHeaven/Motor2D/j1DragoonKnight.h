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

	// Animations of the knight
	Animation attack_up;
	Animation attack_down;
	Animation attack_diagonal_up_right;
	Animation attack_diagonal_up_left;
	Animation attack_diagonal_down_right;
	Animation attack_diagonal_down_left;
	Animation attack_lateral_right;
	Animation attack_lateral_left;

	// Abilities
	j1Timer cooldown_Q;
	uint cooldownTime_Q = 0;
	uint lastTime_Q = 0;
	fPoint lastPosition;
	uint dashSpeed = 0;

	j1Timer cooldown_E;
	uint cooldownTime_E = 0;
	uint lastTime_E = 0;
	j1Timer cooldown_Rage;
	uint duration_Rage = 0;
	uint lastTime_Rage = 0;
	uint rageDamage = 0;

	// Sounds

};

#endif // __j1DRAGOONKNIGHT_H__