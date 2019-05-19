#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1Input.h"
#include "j1BlackMage.h"
#include "j1Render.h"
#include "j1FadeToBlack.h"
#include "j1Window.h"
#include "j1Scene1.h"
#include "j1Scene2.h"
#include "j1Audio.h"
#include "j1Hud.h"
#include "j1Map.h"
#include "j1Timer.h"
#include "j1Particles.h"
#include "j1DialogSystem.h"

#include "Brofiler/Brofiler.h"

j1BlackMage::j1BlackMage(int x, int y, ENTITY_TYPES type) : j1Player(x, y, ENTITY_TYPES::PLAYER)
{
	animation = NULL;

	idle_diagonal_up.LoadAnimation("idleD_up", "mage");
	idle_diagonal_down.LoadAnimation("idleD_down", "mage");
	idle_lateral.LoadAnimation("idleLateral", "mage");
	idle_down.LoadAnimation("idleDown", "mage");
	idle_up.LoadAnimation("idleUp", "mage");

	up.LoadAnimation("up", "mage");
	down.LoadAnimation("down", "mage");
	lateral.LoadAnimation("lateral", "mage");
	diagonal_up.LoadAnimation("diagonalUp", "mage");
	diagonal_down.LoadAnimation("diagonalDown", "mage");
	godmode.LoadAnimation("godmode", "mage");

	i_attack_up.LoadAnimation("i_attackUp", "mage");
	i_attack_down.LoadAnimation("i_attackDown", "mage");
	i_attack_diagonal_up.LoadAnimation("i_attackD_up", "mage");
	i_attack_diagonal_down.LoadAnimation("i_attackD_down", "mage");
	i_attack_lateral.LoadAnimation("i_attackLateral", "mage");

	attack_diagonal_up.LoadAnimation("attackD_up", "mage");
	attack_diagonal_down.LoadAnimation("attackD_down", "mage");
	attack_lateral.LoadAnimation("attackLateral", "mage");
	attack_down.LoadAnimation("attackDown", "mage");
	attack_up.LoadAnimation("attackUp", "mage");

	death.LoadAnimation("death", "mage");
}

j1BlackMage::~j1BlackMage() {}

// Load assets
bool j1BlackMage::Start() {

	// Textures are loaded
	LOG("Loading player textures");
	sprites = App->tex->Load("textures/character/mage/Mage.png");

	LoadPlayerProperties();
	animation = &idle_diagonal_up;

	if (GodMode)
		collider = App->collisions->AddCollider({ (int)position.x + margin.x, (int)position.y + margin.y - 5, playerSize.x, playerSize.y + 5 }, COLLIDER_NONE, App->entity);
	else
		collider = App->collisions->AddCollider({ (int)position.x + margin.x, (int)position.y + margin.y - 5, playerSize.x, playerSize.y + 5 }, COLLIDER_PLAYER, App->entity);

	hud = new j1Hud();
	hud->Start();

	dialog = new j1DialogSystem();
	dialog->Start();

	// Starting ability timers
	cooldown_Q.Start();
	cooldown_E.Start();

	player_start = true;
	return true;
}

//Call modules before each loop iteration
bool j1BlackMage::PreUpdate() {

	BROFILER_CATEGORY("BlackMagePreUpdate", Profiler::Color::Orange)

	return true;
}

// Call modules on each loop iteration
bool j1BlackMage::Update(float dt, bool do_logic) {

	BROFILER_CATEGORY("BlackMageUpdate", Profiler::Color::LightSeaGreen)

	if (player_start)
	{
		current_points = std::to_string(coins);
		// Controls when is finished the dialog
		if (App->scene1->finishedDialog || App->scene2->finishedDialog2) {

			if (!active_Q) {
				ManagePlayerMovement(direction, dt, do_logic, speed);
			}
			if (!attacking)
				SetMovementAnimations(direction, &idle_up, &idle_down, &idle_diagonal_up, &idle_diagonal_down, &idle_lateral,
					&diagonal_up, &diagonal_down, &lateral, &up, &down, &death);

			// ---------------------------------------------------------------------------------------------------------------------
			// COMBAT
			// ---------------------------------------------------------------------------------------------------------------------
			if (GodMode == false && dead == false && changing_room == false && !App->gamePaused) {
				if (!attacking) {
					// Attack control
					if (App->input->GetMouseButtonDown(1) == KEY_DOWN || SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_LEFTSTICK) == KEY_DOWN)
					{
						attacking = true;

						if (App->input->GetMouseButtonDown(1) == KEY_DOWN) {
							iPoint mouse_pos;
							App->input->GetMousePosition(mouse_pos.x, mouse_pos.y);
							Shot(mouse_pos.x, mouse_pos.y, dt);
						}

						if (SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_LEFTSTICK) == KEY_DOWN) {
							fPoint speed_particle;
							fPoint particle_speed = { 250,250 };

							if (animation == &idle_lateral || animation == &lateral) {
								if (facingRight) {
									speed_particle.x = particle_speed.x * cos(0 * DEGTORAD);
									speed_particle.y = particle_speed.y * sin(0 * DEGTORAD);
								}
								else {
									speed_particle.x = particle_speed.x * cos(180 * DEGTORAD);
									speed_particle.y = particle_speed.y * sin(180 * DEGTORAD);
								}
							}
							if (animation == &idle_diagonal_up || animation == &diagonal_up) {
								if (facingRight) {
									speed_particle.x = particle_speed.x * cos(-45 * DEGTORAD);
									speed_particle.y = particle_speed.y * sin(-45 * DEGTORAD);
								}
								else {
									speed_particle.x = particle_speed.x * cos(-135 * DEGTORAD);
									speed_particle.y = particle_speed.y * sin(-135 * DEGTORAD);
								}
							}
							if (animation == &idle_diagonal_down || animation == &diagonal_down) {
								if (facingRight) {
									speed_particle.x = particle_speed.x * cos(-315 * DEGTORAD);
									speed_particle.y = particle_speed.y * sin(-315 * DEGTORAD);
								}
								else {

									speed_particle.x = particle_speed.x * cos(-225 * DEGTORAD);
									speed_particle.y = particle_speed.y * sin(-225 * DEGTORAD);
								}
							}
							if (animation == &idle_up || animation == &up) {
								speed_particle.x = particle_speed.x * cos(-90 * DEGTORAD);
								speed_particle.y = particle_speed.y * sin(-90 * DEGTORAD);
							}
							if (animation == &idle_down || animation == &down) {
								speed_particle.x = particle_speed.x * cos(-270 * DEGTORAD);
								speed_particle.y = particle_speed.y * sin(-270 * DEGTORAD);
							}

							App->particles->mageShot.speed = speed_particle;
							App->particles->mageShot.life = 500;

							App->particles->AddParticle(App->particles->mageShot, position.x + margin.x, position.y + margin.y, dt, COLLIDER_ATTACK);
						}

						if (direction == NONE_) {
							if (animation == &idle_lateral) animation = &i_attack_lateral;
							else if (animation == &idle_up) animation = &i_attack_up;
							else if (animation == &idle_down) animation = &i_attack_down;
							else if (animation == &idle_diagonal_up) animation = &i_attack_diagonal_up;
							else if (animation == &idle_diagonal_down) animation = &i_attack_diagonal_down;
						}
						else {
							if (animation == &lateral) animation = &attack_lateral;
							else if (animation == &up) animation = &attack_up;
							else if (animation == &down) animation = &attack_down;
							else if (animation == &diagonal_up) animation = &attack_diagonal_up;
							else if (animation == &diagonal_down) animation = &attack_diagonal_down;
						}
					}
				}

				if ((cooldown_Q.Read() >= lastTime_Q + cooldownTime_Q) || firstTimeQ) available_Q = true;
				else available_Q = false;

				if ((cooldown_E.Read() >= lastTime_E + cooldownTime_E) || firstTimeE) available_E = true;
				else available_E = false;

				// Fire explosion
				if ((App->input->GetKey(SDL_SCANCODE_Q) == j1KeyState::KEY_DOWN || SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_Y) == KEY_DOWN)
					&& (firstTimeQ || (active_Q == false && cooldown_Q.Read() >= lastTime_Q + cooldownTime_Q))) {

					if (App->dialog->law == 1) App->entity->currentPlayer->lifePoints -= 18;

					iPoint explosionPos;
					iPoint p = { (int)position.x, (int)position.y };

					if (animation == &lateral || animation == &idle_lateral) {
						if (facingRight) explosionPos = { p.x + 20, p.y - 15 };
						else explosionPos = { p.x - 45, p.y - 15 };
					}
					else if (animation == &up || animation == &idle_up) explosionPos = { p.x - 13, p.y - 30 };
					else if (animation == &down || animation == &idle_down)	explosionPos = { p.x - 13, p.y + 4 };
					else if (animation == &diagonal_up || animation == &idle_diagonal_up) {
						if (facingRight) explosionPos = { p.x + 15, p.y - 26 };
						else explosionPos = { p.x - 40, p.y - 26 };
					}
					else if (animation == &diagonal_down || animation == &idle_diagonal_down) {
						if (facingRight) explosionPos = { p.x + 15, p.y - 5 };
						else explosionPos = { p.x - 40, p.y - 5 };
					}

					cooldown_Explosion.Start();
					lastTime_Explosion = cooldown_Explosion.Read();
					App->particles->explosion.anim.Reset();
					App->particles->AddParticle(App->particles->explosion, explosionPos.x, explosionPos.y, dt, COLLIDER_ABILITY);
					App->audio->PlayFx(App->audio->explosion);
					active_Q = true;
					firstTimeQ = false;
				}

				if (active_Q && cooldown_Explosion.Read() >= lastTime_Explosion + duration_Explosion) {

					cooldown_Q.Start();
					lastTime_Q = cooldown_Q.Read();
					active_Q = false;
				}

				// Extra speed
				if ((App->input->GetKey(SDL_SCANCODE_E) == j1KeyState::KEY_DOWN || SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_B) == KEY_DOWN)
					&& (firstTimeE || (active_E == false && cooldown_E.Read() >= lastTime_E + cooldownTime_E))) {

					App->audio->PlayFx(App->audio->rage_bm);

					if (App->dialog->law == 2) App->entity->currentPlayer->lifePoints -= 18;

					speed = speed * 2;
					cooldown_Speed.Start();
					lastTime_Speed = cooldown_Speed.Read();
					active_E = true;
					firstTimeE = false;
				}

				if (active_E && cooldown_Speed.Read() >= lastTime_Speed + duration_Speed) {

					speed = speed / 2;
					cooldown_E.Start();
					lastTime_E = cooldown_E.Read();
					active_E = false;
				}
			}

			// Attack management
			if (attack_lateral.Finished() || attack_up.Finished() || attack_down.Finished()
				|| attack_diagonal_up.Finished() || attack_diagonal_down.Finished()
				|| i_attack_lateral.Finished() || i_attack_up.Finished() || i_attack_down.Finished()
				|| i_attack_diagonal_up.Finished() || i_attack_diagonal_down.Finished() || dead == true) {

				i_attack_lateral.Reset();
				i_attack_up.Reset();
				i_attack_down.Reset();
				i_attack_diagonal_up.Reset();
				i_attack_diagonal_down.Reset();

				attack_lateral.Reset();
				attack_up.Reset();
				attack_down.Reset();
				attack_diagonal_up.Reset();
				attack_diagonal_down.Reset();

				if (animation == &attack_lateral || animation == &i_attack_lateral) animation = &idle_lateral;
				else if (animation == &attack_up || animation == &i_attack_up) animation = &idle_up;
				else if (animation == &attack_down || animation == &i_attack_down) animation = &idle_down;
				else if (animation == &attack_diagonal_up || animation == &i_attack_diagonal_up) animation = &idle_diagonal_up;
				else if (animation == &attack_diagonal_down || animation == &i_attack_diagonal_down) animation = &idle_diagonal_down;
				attacking = false;
			}

			// God mode
			if (App->input->GetKey(SDL_SCANCODE_F10) == j1KeyState::KEY_DOWN && dead == false)
			{
				GodMode = !GodMode;

				if (GodMode == true)
				{
					collider->type = COLLIDER_NONE;
					animation = &godmode;

				}
				else if (GodMode == false)
				{
					collider->type = COLLIDER_PLAYER;
				}
			}
		}
	}

	if (dead) {

		App->audio->PlayFx(App->audio->death_bm);
		animation = &death;
		App->fade->FadeToBlack();

		// Restarting the level in case the player dies
		if (App->fade->IsFading() == 0)
		{
			position = { 200,750 };
			lifePoints = totalLifePoints;
			facingRight = true;

			App->entity->DestroyEntities();

			// Resetting the animation
			death.Reset();
			animation = &idle_diagonal_up;

			dead = false;
		}
	}

	// Checking for the heights
	App->map->EntityMovementTest(this);

	// Update collider position to player position
	if (collider != nullptr)
		collider->SetPos(position.x + margin.x, position.y + margin.y);

	// ---------------------------------------------------------------------------------------------------------------------
	// DRAWING EVERYTHING ON THE SCREEN
	// ---------------------------------------------------------------------------------------------------------------------	

	if (App->input->GetKey(SDL_SCANCODE_0) == KEY_DOWN)
		height = 0.0f;
	else if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		height = 1.0f;
	else if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
		height = 2.0f;
	else if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
		height = 3.0f;
	else if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
		height = 4.0f;
	else if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
		height = 5.0f;
	else if (App->input->GetKey(SDL_SCANCODE_6) == KEY_DOWN)
		height = 6.0f;
	else if (App->input->GetKey(SDL_SCANCODE_7) == KEY_DOWN)
		height = 7.0f;

	return true;
}

bool j1BlackMage::DrawOrder(float dt) {
	// Blitting the player
	SDL_Rect* r = &animation->GetCurrentFrame(dt);

	if (!attacking) {
		if (facingRight || animation == &up || animation == &down || animation == &idle_up || animation == &idle_down)
			Draw(r,false,0,0,1,offset);
		else
			Draw(r, true, 0, 0, 1, offset);
	}
	else {
		if (facingRight || animation == &attack_up || animation == &attack_down || animation == &i_attack_up || animation == &i_attack_down) {
			if (animation == &attack_down || animation == &i_attack_down) Draw(r, false, -4, 0, playerScale, offset);
			else if (animation == &attack_diagonal_down || animation == &i_attack_diagonal_down) Draw(r, false, 0, 2, playerScale, offset);
			else Draw(r, false, 0, 0, playerScale, offset);
		}
		else {
			if (animation == &attack_lateral || animation == &i_attack_lateral) Draw(r, true, -4, 0, playerScale, offset);
			else if (animation == &attack_diagonal_up || animation == &i_attack_diagonal_up) Draw(r, true, -6, 0, playerScale, offset);
			else if (animation == &attack_diagonal_down || animation == &i_attack_diagonal_down) Draw(r, true, -6, 2, playerScale, offset);
			else Draw(r, true, attackBlittingX, attackBlittingY, playerScale, offset);
		}
	}
	return true;
}

// Call modules after each loop iteration
bool j1BlackMage::PostUpdate() {

	BROFILER_CATEGORY("BlackMagePostUpdate", Profiler::Color::Yellow)
	
	dialog->Update(0);

	if (App->scene1->finishedDialog || App->scene2->finishedDialog2)
		hud->Update(0);

	return true;
}

// Load game state
bool j1BlackMage::Load(pugi::xml_node& data) {

	position.x = data.child("player").child("position").attribute("x").as_float();
	position.y = data.child("player").child("position").attribute("y").as_float();

	GodMode = data.child("player").child("godmode").attribute("value").as_bool();

	if (GodMode == true)
	{
		collider->type = COLLIDER_NONE;
		animation = &godmode;
	}
	else if (GodMode == false)
	{
		collider->type = COLLIDER_PLAYER;
	}

	if (hud)
		hud->Load(data);

	return true;
}

// Save game state
bool j1BlackMage::Save(pugi::xml_node& data) const {

	pugi::xml_node pos = data.append_child("position");

	pos.append_attribute("x") = position.x;
	pos.append_attribute("y") = position.y;

	pugi::xml_node godmode = data.append_child("godmode");

	pugi::xml_node life = data.append_child("lives");

	godmode.append_attribute("value") = GodMode;

	if (hud)
		hud->Save(data.append_child("hud"));

	return true;
}

// Called before quitting
bool j1BlackMage::CleanUp() {

	// Remove all memory leaks
	LOG("Unloading the player");
	App->tex->UnLoad(sprites);

	if (collider != nullptr)
		collider->to_delete = true;

	if (hud)
		hud->CleanUp();

	RELEASE(hud);

	return true;
}

void j1BlackMage::LoadPlayerProperties() {

	pugi::xml_document config_file;
	config_file.load_file("config.xml");
	pugi::xml_node config;
	config = config_file.child("config");
	pugi::xml_node player;
	player = config.child("player").child("blackMage");

	// Copying the size of the player
	playerSize.x = player.child("size").attribute("width").as_int();
	playerSize.y = player.child("size").attribute("height").as_int();
	margin.x = player.child("margin").attribute("x").as_int();
	margin.y = player.child("margin").attribute("y").as_int();
	offset = player.child("margin").attribute("offset").as_int();
	playerScale = player.attribute("scale").as_float();

	// Copying attack values
	attackBlittingX = player.child("attack").attribute("blittingX").as_int();
	attackBlittingY = player.child("attack").attribute("blittingY").as_int();

	// Copying combat values
	pugi::xml_node combat = player.child("combat");
	pugi::xml_node cd = player.child("cooldowns");

	basicDamage = combat.attribute("basicDamage").as_int();
	fireDamage = combat.attribute("fireDamage").as_int();
	lifePoints = combat.attribute("lifePoints").as_int();
	totalLifePoints = combat.attribute("lifePoints").as_int();
	knockback = combat.attribute("knockback").as_int();
	cooldownTime_Q = cd.attribute("Q").as_uint();
	duration_Explosion = cd.attribute("explosion").as_uint();
	cooldownTime_E = cd.attribute("E").as_uint();
	duration_Speed = cd.attribute("increasedSpeed").as_uint();
	invulTime = cd.attribute("invulTime").as_uint();

	// Copying values of the speed
	pugi::xml_node movementSpeed = player.child("speed");

	speed = movementSpeed.child("movement").attribute("horizontal").as_float();
	godModeSpeed = movementSpeed.child("movement").attribute("godmode").as_float();
}

void j1BlackMage::Shot(float x, float y, float dt) {

	// To change where the particle is born
	fPoint margin;
	margin.x = 8;
	margin.y = 8;

	fPoint edge;
	edge.x = x - (position.x + margin.x) - (App->render->camera.x / (int)App->win->GetScale());
	edge.y = (position.y + margin.y) - y + (App->render->camera.y / (int)App->win->GetScale());

	// If the map is very big and its not enough accurate, we should use long double for the var angle
	double angle = -(atan2(edge.y, edge.x));

	fPoint speed_particle;
	fPoint p_speed = { 250,250 };

	speed_particle.x = p_speed.x * cos(angle);
	speed_particle.y = p_speed.y * sin(angle);
	App->particles->mageShot.speed = speed_particle;

	App->particles->AddParticle(App->particles->mageShot, position.x + margin.x, position.y + margin.y, dt, COLLIDER_ATTACK);
	App->audio->PlayFx(App->audio->attack_bm);
}