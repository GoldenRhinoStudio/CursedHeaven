#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2Point.h"
#include "p2Animation.h"
#include "j1Entity.h"
#include "j1Timer.h"

struct SDL_Texture;
struct Collider;
class j1Hud;

class j1Player : public j1Entity
{

public:
	j1Player(int x, int y, ENTITY_TYPES type);

	// Destructor
	virtual ~j1Player();

	// Called before the first frame
	virtual bool Start() {
		return true;
	};

	// Called each loop iteration
	virtual bool PreUpdate() {
		return true;
	};
	virtual bool Update(float dt, bool do_logic) {
		return true;
	};
	virtual bool PostUpdate() {
		return true;
	};

	// Called before quitting
	virtual bool CleanUp() {
		return true;
	};

	// Called to check collisions
	void OnCollision(Collider* col_1, Collider* col_2);

	// Load / Save
	virtual bool Load(pugi::xml_node&) {
		return true;
	};
	virtual bool Save(pugi::xml_node&) const {
		return true;
	};

	virtual void LoadPlayerProperties() {};
	void UpdateCameraPosition(float dt);
	void ManagePlayerMovement(DIRECTION& direction, float dt, bool do_logic, float speed);
	void SetMovementAnimations(DIRECTION& direction, Animation* idle_up, Animation* idle_down, Animation* idle_diagonal_up, Animation* idle_diagonal_down, Animation* idle_lateral,
		Animation* diagonal_up, Animation* diagonal_down, Animation* lateral, Animation* go_up, Animation* go_down, Animation* death);
	void ChangeRoom(int x, int y);

public:

	// To know the last direction the character was moving to
	bool facingRight = true;

	// Size of the player collider, where x = w and y = h
	iPoint playerSize;
	iPoint attackSize;
	iPoint margin;

	Collider* attackCollider = nullptr;

	int room = 6;
	uint points = 0;
	uint score_points = 0;
	uint playerLife = 0;
	uint coins = 0;

	j1Hud* hud = nullptr;

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

	int attackBlittingX;
	int attackBlittingY;
	int rightAttackSpawnPos;
	int leftAttackSpawnPos;
	float godModeSpeed;

	bool player_start = false;
	bool dead = false;
	bool victory = false;
	bool loadedAudios = false;
	bool changing_room = false;
	bool receivedDamage = false;
	bool GodMode = false;

	// Combat
	bool attacking = false;
	bool available_Q = false;
	bool active_Q = false;
	bool firstTimeQ = true;
	bool available_E = false;
	bool active_E = false;
	bool firstTimeE = true;

	uint cooldownTime_Q = 0;
	uint cooldownTime_E = 0;
	uint lastTime_Q = 0;
	uint lastTime_E = 0;

	// The player stays untouchable for a second
	j1Timer invulCounter;
	uint lastTime_invul = 0;
	uint invulTime = 0;
	
};

#endif // __j1PLAYER_H__