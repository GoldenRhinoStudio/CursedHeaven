#ifndef __j1PARTICLES_H__
#define __j1PARTICLES_H__

#include "j1Module.h"
#include "p2Animation.h"
#include "p2Point.h"
#include "j1Collisions.h"
#include "p2Log.h"

#define MAX_ACTIVE_PARTICLES 100

struct SDL_Texture;
struct Collider;
enum COLLIDER_TYPE;

struct Particle
{
	SDL_Texture * tex = nullptr;
	int type_particle;
	Collider* collider = nullptr;
	Animation anim;
	uint fx = 0;
	fPoint position;
	fPoint speed;
	Uint32 born = 0;
	Uint32 life = 0;
	uint32 state = 0;
	bool fx_played = false;
	Particle();
	Particle(const Particle& p);
	virtual ~Particle();
	bool Update(float dt);
};

class j1Particles : public j1Module
{
public:
	j1Particles();
	virtual ~j1Particles();
	bool Start();
	bool Update(float dt);
	bool CleanUp();
	void OnCollision(Collider* c1, Collider* c2);
	void AddParticle(const Particle& particle, int x, int y, float dt, COLLIDER_TYPE collider_type = COLLIDER_NONE, Uint32 delay = 0);
	Particle* active[MAX_ACTIVE_PARTICLES];
private:
	
	SDL_Texture* part_tex = nullptr;
	SDL_Texture* sword_tex = nullptr;
	int swordcounter = 0;

public:
	uint width, height;

	// Black Mage particles
	Particle mageShot;
	Particle explosion;

	Particle sword1;
	Particle sword2;
	Particle sword3;

};
#endif // __j1PARTICLES_H__ 