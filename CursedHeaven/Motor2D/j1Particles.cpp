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
#include "Exodus.h"

#include "Brofiler/Brofiler.h"

j1Particles::j1Particles()
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
		active[i] = nullptr;

	// Mage basic attack
	mageShot.anim.LoadAnimation("shot", "mage");
	mageShot.life = 1000;

	// Mage Q
	explosion.anim.LoadAnimation("explosion", "mage");
	explosion.life = 570;

	//EXODUS Sword1
	sword1.anim.LoadAnimation("sword_attack1", "exodus",false);
	sword1.life = 100000;
	sword2.anim.LoadAnimation("sword_attack2", "exodus", false);
	sword2.life = 100000;
	sword3.anim.LoadAnimation("sword_attack3", "exodus", false);
	sword3.life = 100000;
}

j1Particles::~j1Particles()
{}

// Load assets
bool j1Particles::Start()
{
	LOG("Loading particles");
	part_tex = App->tex->Load("textures/character/particles.png");
	sword_tex = App->tex->Load("textures/Effects/Particle effects/wills_magic_pixel_particle_effects/sword_burst/spritesheet.png");


	mageShot.tex = part_tex;
	sword1.tex = sword_tex;
	sword2.tex = sword_tex;
	sword3.tex = sword_tex;

	return true;
}

bool j1Particles::CleanUp()
{
	LOG("Unloading particles");
	App->tex->UnLoad(part_tex);
	App->tex->UnLoad(sword_tex);
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
	BROFILER_CATEGORY("ParticlesUpdate", Profiler::Color::LightSeaGreen)

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
			App->render->Blit(p->tex, p->position.x, p->position.y, &(p->anim.GetCurrentFrame(dt)));

			
			if (p->fx_played == false)
			{
				p->fx_played = true;
				//Play your fx here
			}

			if (App->entity->exodus != nullptr) {
				if (p->anim.isLastFrame() && p->state == 0) {
					Particle* aux = new Particle(sword2);
					aux->anim.Reset();
					aux->born = SDL_GetTicks();
					aux->position.x = p->position.x;
					aux->position.y = p->position.y;
					aux->collider = App->collisions->AddCollider(aux->anim.GetCurrentFrame(dt), COLLIDER_ENEMY_SHOT, this);
					aux->state = App->entity->exodus->state;
					active[i] = aux;
					delete p;
				}
				else if (p->anim.isLastFrame() && p->state == 1) {
					Particle* aux = new Particle(sword3);
					aux->anim.Reset();
					aux->born = SDL_GetTicks();
					aux->position.x = p->position.x;
					aux->position.y = p->position.y;
					aux->collider = App->collisions->AddCollider(aux->anim.GetCurrentFrame(dt), COLLIDER_ENEMY_SHOT, this);
					aux->state = 2;
					active[i] = aux;
					delete p;
				}
				else if (p->anim.isLastFrame() && p->state == 2) {
					delete p;
					active[i] = nullptr;
				}
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
			p->state = 0;
			p->anim.Reset();
			if (collider_type != COLLIDER_NONE) {
				p->collider = App->collisions->AddCollider(p->anim.GetCurrentFrame(dt), collider_type, this);
			}
			Collider* test = p->collider;
			active[i] = p;
			break;
		}
	}
}

void j1Particles::OnCollision(Collider* c1, Collider* c2)
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		// Always destroy particles that collide
		if (active[i] != nullptr && active[i]->collider == c1)
		{
			//AddParticle(...) ---> If we want to print an explosion when hitting an enemy
			delete active[i];
			active[i] = nullptr;
			break;
		
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
	fx(p.fx), born(p.born), life(p.life), tex(p.tex)
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