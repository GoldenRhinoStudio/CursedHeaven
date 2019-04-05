#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1Input.h"
#include "j1DragoonKnight.h"
#include "j1Render.h"
#include "j1FadeToBlack.h"
#include "j1Audio.h"
#include "j1Hud.h"
#include "j1Map.h"
#include "j1Timer.h"

#include "Brofiler/Brofiler.h"

j1DragoonKnight::j1DragoonKnight(int x, int y, ENTITY_TYPES type) : j1Player(x, y, ENTITY_TYPES::PLAYER)
{
	animation = NULL;

	idle_diagonal_up.LoadAnimation("idleD_up", "knight");
	idle_diagonal_down.LoadAnimation("idleD_down", "knight");
	idle_lateral.LoadAnimation("idleLateral", "knight");
	idle_down.LoadAnimation("idleDown", "knight");
	idle_up.LoadAnimation("idleUp", "knight");

	up.LoadAnimation("up", "knight");
	down.LoadAnimation("down", "knight");
	lateral.LoadAnimation("lateral", "knight");
	diagonal_up.LoadAnimation("diagonalUp", "knight");
	diagonal_down.LoadAnimation("diagonalDown", "knight");
	godmode.LoadAnimation("godmode", "knight");

	attack_diagonal_up.LoadAnimation("attackD_up", "knight");
	attack_diagonal_down.LoadAnimation("attackD_down", "knight");
	attack_lateral.LoadAnimation("attackLateral", "knight");
	attack_down.LoadAnimation("attackDown", "knight");
	attack_up.LoadAnimation("attackUp", "knight");
}

j1DragoonKnight::~j1DragoonKnight() {}

// Load assets
bool j1DragoonKnight::Start() {

	// Textures are loaded
	LOG("Loading player textures");
	sprites = App->tex->Load("textures/character/dragoonknight/Dragoon.png");

	// Audios are loaded
	LOG("Loading player audios");
	if (!loadedAudios) {
		loadedAudios = true;
	}

	LoadPlayerProperties();
	animation = &idle_diagonal_up;

	// Setting player position
	position.x = 200;
	position.y = 750;

	if (GodMode)
		collider = App->collisions->AddCollider({ (int)position.x + margin.x, (int)position.y + margin.y, playerSize.x, playerSize.y }, COLLIDER_NONE, App->entity);
	else
		collider = App->collisions->AddCollider({ (int)position.x + margin.x, (int)position.y + margin.y, playerSize.x, playerSize.y }, COLLIDER_PLAYER, App->entity);

	attackCollider = App->collisions->AddCollider({ (int)position.x + rightAttackSpawnPos, (int)position.y + margin.y, playerSize.x, playerSize.y }, COLLIDER_NONE, App->entity);

	hud = new j1Hud();
	hud->Start();

	// Starting ability timers
	cooldown_Q.Start();
	cooldown_E.Start();

	player_start = true;
	return true;
}

//Call modules before each loop iteration
bool j1DragoonKnight::PreUpdate() {

	BROFILER_CATEGORY("DragoonKnightPreUpdate", Profiler::Color::Orange)

	return true;
}

// Call modules on each loop iteration
bool j1DragoonKnight::Update(float dt, bool do_logic) {

	BROFILER_CATEGORY("DragoonKnightUpdate", Profiler::Color::LightSeaGreen)

	if (player_start)
	{
		if (!attacking && !active_Q) {
			ManagePlayerMovement(App->entity->knight, dt);
			SetMovementAnimations(&idle_up, &idle_down, &idle_diagonal_up, &idle_diagonal_down, &idle_lateral,
				&diagonal_up, &diagonal_down, &lateral, &up, &down);
		}

		// ---------------------------------------------------------------------------------------------------------------------
		// COMBAT
		// ---------------------------------------------------------------------------------------------------------------------
		if (GodMode == false && dead == false) {
			if (!attacking) {
				// Attack control
				if ((App->input->GetKey(SDL_SCANCODE_P) == j1KeyState::KEY_DOWN || SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_X) == KEY_DOWN)) {
					attacking = true;
					attackCollider->type = COLLIDER_ATTACK;

					if (animation == &lateral || animation == &idle_lateral) animation = &attack_lateral;
					else if (animation == &up || animation == &idle_up) animation = &attack_up;
					else if (animation == &down || animation == &idle_down)	animation = &attack_down;
					else if (animation == &diagonal_up || animation == &idle_diagonal_up) animation = &attack_diagonal_up;
					else if (animation == &diagonal_down || animation == &idle_diagonal_down) animation = &attack_diagonal_down;
				}

				// Ability control
				if ((App->input->GetKey(SDL_SCANCODE_Q) == j1KeyState::KEY_DOWN || SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_Y) == KEY_DOWN)
					&& active_Q == false && cooldown_Q.Read() >= lastTime_Q + 5000) {

					lastPosition = position;

					if (direction != NONE_) active_Q = true;
				}

				if (active_Q) {

					if (direction == RIGHT_ && (position.x <= lastPosition.x + 100.0f))
						position.x += dashSpeed * dt;
					else if (direction == LEFT_ && (position.x >= lastPosition.x - 100.0f))
						position.x -= dashSpeed * dt;
					else if (direction == UP_ && (position.y >= lastPosition.y - 100.0f))
						position.y -= dashSpeed * dt;
					else if (direction == DOWN_ && (position.y <= lastPosition.y + 100.0f))
						position.y += dashSpeed * dt;
					else if (direction == UP_LEFT_ && (position.x >= lastPosition.x - 120.0f) && (position.y >= lastPosition.y - 60.0f)) {
						position.x -= dashSpeed * dt;
						position.y -= dashSpeed * dt;
					}
					else if (direction == UP_RIGHT_ && (position.x <= lastPosition.x + 120.0f) && (position.y >= lastPosition.y - 60.0f)) {
						position.x += dashSpeed * dt;
						position.y -= dashSpeed * dt;
					}
					else if (direction == DOWN_LEFT_ && (position.x >= lastPosition.x - 120.0f) && (position.y <= lastPosition.y + 60.0f)) {
						position.x -= dashSpeed * dt;
						position.y += dashSpeed * dt;
					}
					else if (direction == DOWN_RIGHT_ && (position.x <= lastPosition.x + 120.0f) && (position.y <= lastPosition.y + 60.0f)) {
						position.x += dashSpeed * dt;
						position.y += dashSpeed * dt;
					}
					else {
						cooldown_Q.Start();
						lastTime_Q = cooldown_Q.Read();
						active_Q = false;
					}
				}
			}

			if ((App->input->GetKey(SDL_SCANCODE_E) == j1KeyState::KEY_DOWN || SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_B) == KEY_DOWN)
				&& active_E == false && cooldown_E.Read() >= lastTime_E + 25000) {

				basicDamage += rageDamage;
				cooldown_Rage.Start();
				lastTime_Rage =	cooldown_Rage.Read();
				active_E = true;
			}

			if (active_E && cooldown_Rage.Read() >= lastTime_Rage + 5000) {

				basicDamage -= rageDamage;
				cooldown_E.Start();
				lastTime_E = cooldown_Q.Read();
				active_E = false;
			}
		}

		// Attack management
		if (attack_lateral.Finished() || attack_up.Finished() || attack_down.Finished() ||
			attack_diagonal_up.Finished() || attack_diagonal_down.Finished() || dead == true) {

			attackCollider->type = COLLIDER_NONE;

			attack_lateral.Reset();
			attack_up.Reset();
			attack_down.Reset();
			attack_diagonal_up.Reset();
			attack_diagonal_down.Reset();

			if (animation == &attack_lateral) animation = &idle_lateral;
			else if (animation == &attack_up) animation = &idle_up;
			else if (animation == &attack_down) animation = &idle_down;
			else if (animation == &attack_diagonal_up) animation = &idle_diagonal_up;
			else if (animation == &attack_diagonal_down) animation = &idle_diagonal_down;
			attacking = false;
		}
		else if (attackCollider != nullptr) {
			if (facingRight)
				attackCollider->SetPos((int)position.x + rightAttackSpawnPos, (int)position.y + margin.y);
			else
				attackCollider->SetPos((int)position.x + leftAttackSpawnPos, (int)position.y + margin.y);
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
	if (dead) {

		// Restarting the level in case the player dies
		if (App->fade->IsFading() == 0)
		{
			facingRight = true;

			App->entity->DestroyEntities();

			// Resetting the animation
			death.Reset();
			attack_lateral.Reset();
			animation = &idle_diagonal_up;

			dead = false;
		}
	}

	// Checking for the heights
	App->map->EntityMovement(App->entity->knight);

	// Update collider position to player position
	if (collider != nullptr)
		collider->SetPos(position.x + margin.x, position.y + margin.y);

	// ---------------------------------------------------------------------------------------------------------------------
	// DRAWING EVERYTHING ON THE SCREEN
	// ---------------------------------------------------------------------------------------------------------------------	

	// Blitting the player
	SDL_Rect r = animation->GetCurrentFrame(dt);

	if (!attacking) {
		if (facingRight || animation == &up || animation == &down || animation == &idle_up || animation == &idle_down)
			Draw(r);
		else
			Draw(r, true);
	}
	else {
		if (facingRight || animation == &attack_up || animation == &attack_down) {
			if (animation == &attack_lateral) Draw(r, false, 0, -6);
			else if (animation == &attack_up) Draw(r, false, 0, -2);
			else if (animation == &attack_down) Draw(r, false, -4, -4);
			else if (animation == &attack_diagonal_down) Draw(r, false, 0, -6);
			else Draw(r, false, 0, attackBlittingY);
		}
		else {
			if (animation == &attack_lateral) Draw(r, true, 0, -6);
			else if (animation == &attack_diagonal_down) Draw(r, true, -4, -6);
			else Draw(r, true, attackBlittingX, attackBlittingY);
		}
	}

	hud->Update(dt);

	// We update the camera to follow the player every frame
	 UpdateCameraPosition(dt);

	return true;
}

// Call modules after each loop iteration
bool j1DragoonKnight::PostUpdate() {

	BROFILER_CATEGORY("DragoonKnightPostUpdate", Profiler::Color::Yellow)

		return true;
}

// Load game state
bool j1DragoonKnight::Load(pugi::xml_node& data) {

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
bool j1DragoonKnight::Save(pugi::xml_node& data) const {

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
bool j1DragoonKnight::CleanUp() {

	// Remove all memory leaks
	LOG("Unloading the player");
	App->tex->UnLoad(sprites);

	if (collider != nullptr)
		collider->to_delete = true;

	if (attackCollider != nullptr)
		attackCollider->to_delete = true;

	if (hud)
		hud->CleanUp();

	RELEASE(hud);

	return true;
}

void j1DragoonKnight::LoadPlayerProperties() {

	pugi::xml_document config_file;
	config_file.load_file("config.xml");
	pugi::xml_node config;
	config = config_file.child("config");
	pugi::xml_node player;
	player = config.child("player");

	// Copying the size of the player
	playerSize.x = player.child("size").attribute("width").as_int();
	playerSize.y = player.child("size").attribute("height").as_int();
	margin.x = player.child("margin").attribute("x").as_int();
	margin.y = player.child("margin").attribute("y").as_int();

	// Copying attack values
	attackBlittingX = player.child("attack").attribute("blittingX").as_int();
	attackBlittingY = player.child("attack").attribute("blittingY").as_int();
	rightAttackSpawnPos = player.child("attack").attribute("rightColliderSpawnPos").as_int();
	leftAttackSpawnPos = player.child("attack").attribute("leftColliderSpawnPos").as_int();

	// Copying attackcombat values
	basicDamage = player.child("combat").attribute("basicDamage").as_uint();
	rageDamage = player.child("combat").attribute("rageDamage").as_uint();
	dashSpeed= player.child("combat").attribute("dashSpeed").as_uint();

	// Copying values of the speed
	pugi::xml_node speed = player.child("speed");

	horizontalSpeed = speed.child("movement").attribute("horizontal").as_float();
	godModeSpeed = speed.child("movement").attribute("godmode").as_float();
}