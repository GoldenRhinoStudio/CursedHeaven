#ifndef __j1COLLISIONS_H__
#define __j1COLLISIONS_H__

#include "j1Module.h"
#include "p2Point.h"
#include "SDL\include\SDL_rect.h"

#define MAX_COLLIDERS 600

enum COLLIDER_TYPE
{
	COLLIDER_NONE,
	COLLIDER_WALL,
	COLLIDER_HOOK,
	COLLIDER_PLAYER,
	COLLIDER_ENEMY,
	COLLIDER_DEATH,
	COLLIDER_WIN,
	COLLIDER_ATTACK,
	COLLIDER_COIN,
	COLLIDER_MAX,
};

enum COLLISION_DIRECTION
{
	NONE_COLLISION,
	UP_COLLISION,
	DOWN_COLLISION,
	RIGHT_COLLISION,
	LEFT_COLLISION
};

struct Collider
{
	SDL_Rect rect;
	bool to_delete = false;
	COLLIDER_TYPE type;

	j1Module* callback = nullptr;

	Collider(SDL_Rect rectangle, COLLIDER_TYPE type, j1Module* callback = nullptr) : rect(rectangle), type(type), callback(callback) {}

	void SetPos(int x, int y) { rect.x = x; rect.y = y; }
	bool CheckCollision(const SDL_Rect& r) const;
	COLLISION_DIRECTION CheckDirection(const SDL_Rect& r) const;
};

class j1Collisions : public j1Module
{
public:

	j1Collisions();
	~j1Collisions();

	bool PreUpdate();
	bool Update(float dt);
	bool CleanUp();

	Collider* AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback = nullptr);
	void DrawColliders();
	bool debug = false;


private:

	Collider* colliders[MAX_COLLIDERS];
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];
};


#endif // __j1COLLISIONS_H__