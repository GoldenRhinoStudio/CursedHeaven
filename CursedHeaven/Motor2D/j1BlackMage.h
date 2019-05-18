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
	bool DrawOrder(float dt);
	bool PostUpdate();

	void LoadPlayerProperties();

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	// Called before quitting
	bool CleanUp();

private:
	void Shot(float x, float y, float dt);

public:

	// Animations of the mage
	Animation attack_up;
	Animation attack_down;
	Animation attack_diagonal_up;
	Animation attack_diagonal_down;
	Animation attack_lateral;

	Animation i_attack_up;
	Animation i_attack_down;
	Animation i_attack_diagonal_up;
	Animation i_attack_diagonal_down;
	Animation i_attack_lateral;

	// Abilities
	j1Timer cooldown_Explosion;
	uint duration_Explosion = 0;
	uint lastTime_Explosion = 0;
	j1Timer cooldown_Q;
	int fireDamage = 0;

	j1Timer cooldown_E;
	j1Timer cooldown_Speed;
	uint duration_Speed = 0;
	uint lastTime_Speed = 0;

	// Sounds
	uint deathSound;
	uint playerHurt;
	uint moveSound;
	uint attackSound;
	uint qability;
	uint eability;

};

#endif // __j1BLACKMAGE_H__