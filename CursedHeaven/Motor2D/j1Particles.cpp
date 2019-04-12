#include "j1App.h"
#include "j1Particles.h"
#include "j1Map.h"
#include "j1Scene1.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Input.h"
#include "j1Player.h"
#include "p2Animation.h"
#include "j1Scene1.h"

j1Particles::j1Particles()
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
		active[i] = nullptr;
}

j1Particles::~j1Particles()
{}

// Load assets
bool j1Particles::Start()
{
	LOG("Loading particles");
	part_tex = App->tex->Load("textures/character/particles.png");

	// Mage basic attack
	shot_right.anim.LoadAnimation("shot", "mage");
	shot_right.life = 2500;

	// Mage Q
	explosion.anim.LoadAnimation("explosion", "mage");
	explosion.life = 570;

	return true;
}

bool j1Particles::CleanUp()
{
	LOG("Unloading particles");
	App->tex->UnLoad(part_tex);
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		if (active[i] != nullptr)
		{
			delete active[i];
			active[i] = nullptr;
		}
	}
	return true;
}

bool j1Particles::Update(float dt)
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		Particle* p = active[i];

		if (p == nullptr)
			continue;

		if (p->Update(dt) == false)
		{
			delete p;
			active[i] = nullptr;
		}
		else if (SDL_GetTicks() >= p->born)
		{
			App->render->Blit(part_tex, p->position.x, p->position.y, &(p->anim.GetCurrentFrame(dt)));

			if (p->fx_played == false)
			{
				p->fx_played = true;
				//Play your fx here
			}
		}
	}

	return true;
}

void j1Particles::AddParticle(const Particle& particle, int x, int y, float dt, COLLIDER_TYPE collider_type, Uint32 delay)
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		if (active[i] == nullptr)
		{
			Particle* p = new Particle(particle);
			p->born = SDL_GetTicks() + delay;
			p->position.x = x;
			p->position.y = y;
			if (collider_type != COLLIDER_NONE) {
				p->collider = App->collisions->AddCollider(p->anim.GetCurrentFrame(dt), collider_type, this);
			}
			Collider* test = p->collider;
			active[i] = p;
			break;
		}
	}
}

// every time a particle hits a wall it triggers an explosion particle
void j1Particles::OnCollision(Collider* c1, Collider* c2)
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		// Always destroy particles that collide
		if (active[i] != nullptr && active[i]->collider == c1)
		{
			if (active[i]->anim.Finished()) {
				delete active[i];
				active[i] = nullptr;
				break;
			}
		}
	}
}


// -------------------------------------------------------------
// -------------------------------------------------------------
Particle::Particle()
{
	position.SetToZero();
	speed.SetToZero();
}

Particle::Particle(const Particle& p) :
	anim(p.anim), position(p.position), speed(p.speed),
	fx(p.fx), born(p.born), life(p.life)
{}

Particle::~Particle()
{
	if (collider != nullptr)
		collider->to_delete = true;
}

bool Particle::Update(float dt)
{
	bool ret = true;
	if (life > 0)
	{
		if ((SDL_GetTicks() - born) > life)
			ret = false;
	}

	position.x += speed.x * dt;
	position.y += speed.y * dt;


	if (collider != nullptr)
		collider->SetPos(position.x, position.y);

	return ret;
}