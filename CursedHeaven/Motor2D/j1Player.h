#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2Point.h"
#include "p2Animation.h"
#include "j1Entity.h"

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
	void UpdateCameraPosition();
	bool CheckWalkability(iPoint pos) const;
	void ManagePlayerMovement(j1Player* currentPlayer, float dt, Animation* idle_up, Animation* idle_down, Animation* idle_diagonal_up, Animation* idle_diagonal_down, Animation* idle_lateral,
		Animation* diagonal_up, Animation* diagonal_down, Animation* lateral, Animation* go_up, Animation* go_down);

public:

	// To know the last direction the character was moving to
	bool facingRight = true;

	// Size of the player collider, where x = w and y = h
	iPoint playerSize;
	iPoint margin;

	uint points = 0;
	uint score_points = 0;

	Collider* attackCollider = nullptr;

	j1Hud* hud = nullptr;

	// Attack values
	int attackBlittingX;
	int attackBlittingY;
	int rightAttackSpawnPos;
	int leftAttackSpawnPos;

	float godModeSpeed;
	float horizontalSpeed;

	bool player_start = false;
	bool dead = false;
	bool playedSound = false;
	bool attacking = false;
	bool loadedAudios = false;
	bool GodMode = false;
};

#endif // __j1PLAYER_H__