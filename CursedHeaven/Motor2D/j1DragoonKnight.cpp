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
#include "j1Scene1.h"
#include "j1Audio.h"
#include "j1DialogSystem.h"

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

	attack_diagonal_up_right.LoadAnimation("attackD_up_right", "knight");
	attack_diagonal_up_left.LoadAnimation("attackD_up_left", "knight");
	attack_diagonal_down_right.LoadAnimation("attackD_down_right", "knight");
	attack_diagonal_down_left.LoadAnimation("attackD_down_left", "knight");
	attack_lateral_right.LoadAnimation("attackLateral_right", "knight");
	attack_lateral_left.LoadAnimation("attackLateral_left", "knight");
	attack_down.LoadAnimation("attackDown", "knight");
	attack_up.LoadAnimation("attackUp", "knight");

	death.LoadAnimation("death", "knight");
}

j1DragoonKnight::~j1DragoonKnight() {}

// Load assets
bool j1DragoonKnight::Start() {

	// Textures are loaded
	LOG("Loading player textures");
	sprites = App->tex->Load("textures/character/dragoonknight/Dragoon.png");
	enraged = App->tex->Load("textures/character/dragoonknight/DragoonRage.png");

	// Audios are loaded
	LOG("Loading player audios");
	if (!loadedAudios) {
		loadedAudios = true;
	}

	LoadPlayerProperties();
	animation = &idle_diagonal_up;

	/*position.x = -1050;
	position.y = 750;*/
	position.x = 180;
	position.y = 770;

	/*position.x = -200;
	position.y = 1200;*/

	//coins = 100;

	if (GodMode)
		collider = App->collisions->AddCollider({ (int)position.x + margin.x, (int)position.y + margin.y, playerSize.x, playerSize.y}, COLLIDER_NONE, App->entity);
	else
		collider = App->collisions->AddCollider({ (int)position.x + margin.x, (int)position.y + margin.y, playerSize.x, playerSize.y}, COLLIDER_PLAYER, App->entity);

	attackCollider = App->collisions->AddCollider({ (int)position.x + rightAttackSpawnPos, (int)position.y + margin.y + 5, attackSize.x, attackSize.y}, COLLIDER_NONE, App->entity);

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
bool j1DragoonKnight::PreUpdate() {

	BROFILER_CATEGORY("DragoonKnightPreUpdate", Profiler::Color::Orange)

	return true;
}

// Call modules on each loop iteration
bool j1DragoonKnight::Update(float dt, bool do_logic) {

	BROFILER_CATEGORY("DragoonKnightUpdate", Profiler::Color::LightSeaGreen)

	if (player_start)
	{
		if (App->scene1->finishedDialog) {
			
			if (!attacking && !active_Q) {
				ManagePlayerMovement(direction, dt, do_logic, movementSpeed);
				SetMovementAnimations(direction, &idle_up, &idle_down, &idle_diagonal_up, &idle_diagonal_down, &idle_lateral,
					&diagonal_up, &diagonal_down, &lateral, &up, &down, &death);
			}

			// ---------------------------------------------------------------------------------------------------------------------
			// COMBAT
			// ---------------------------------------------------------------------------------------------------------------------
			if (GodMode == false && dead == false && changing_room == false && !App->gamePaused) {
				if (!attacking) {
					// Attack control
					if ((App->input->GetMouseButtonDown(1) == KEY_DOWN || SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_LEFTSTICK) == KEY_DOWN)) {

						App->audio->PlayFx(App->audio->attack_dk);
						attacking = true;
						attackCollider->type = COLLIDER_ATTACK;

						if (animation == &lateral || animation == &idle_lateral) {
							if (facingRight) animation = &attack_lateral_right;
							else animation = &attack_lateral_left;
						}
						else if (animation == &up || animation == &idle_up) animation = &attack_up;
						else if (animation == &down || animation == &idle_down)	animation = &attack_down;
						else if (animation == &diagonal_up || animation == &idle_diagonal_up) {
							if(facingRight) animation = &attack_diagonal_up_right;
							else animation = &attack_diagonal_up_left;
						}
						else if (animation == &diagonal_down || animation == &idle_diagonal_down) {
							if (facingRight) animation = &attack_diagonal_down_right;
							else animation = &attack_diagonal_down_left;
						}
					}

					if ((cooldown_Q.Read() >= lastTime_Q + cooldownTime_Q) || firstTimeQ) available_Q = true;
					else available_Q = false;

					if ((cooldown_E.Read() >= lastTime_E + cooldownTime_E) || firstTimeE) available_E = true;
					else available_E = false;

					// Ability control
					if ((App->input->GetKey(SDL_SCANCODE_Q) == j1KeyState::KEY_DOWN || SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_Y) == KEY_DOWN)
						&& (firstTimeQ || (active_Q == false && cooldown_Q.Read() >= lastTime_Q + cooldownTime_Q))) {

						App->audio->PlayFx(App->audio->dash);

						if (dialog->law == 1) App->entity->currentPlayer->lifePoints -= 76;				

						lastPosition = position;

						if (direction != NONE_) {
							active_Q = true;
							firstTimeQ = false;
						}
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
					&& (firstTimeE || (active_E == false && cooldown_E.Read() >= lastTime_E + cooldownTime_E))) {

					App->audio->PlayFx(App->audio->rage_dk);

					if (dialog->law == 2) App->entity->currentPlayer->lifePoints -= 76;

					basicDamage += rageDamage;
					cooldown_Rage.Start();
					lastTime_Rage =	cooldown_Rage.Read();
					active_E = true;
					firstTimeE = false;
				}

				if (active_E && cooldown_Rage.Read() >= lastTime_Rage + duration_Rage) {

					basicDamage -= rageDamage;
					cooldown_E.Start();
					lastTime_E = cooldown_E.Read();
					active_E = false;
				}
			}


			// Attack management
			if (attack_lateral_right.Finished() || attack_lateral_left.Finished() || attack_up.Finished() || attack_down.Finished() || attack_diagonal_up_right.Finished() || attack_diagonal_up_left.Finished()
				|| attack_diagonal_down_right.Finished() || attack_diagonal_down_left.Finished() || dead == true) {

				attackCollider->type = COLLIDER_NONE;

				attack_lateral_right.Reset();
				attack_lateral_left.Reset();
				attack_up.Reset();
				attack_down.Reset();
				attack_diagonal_up_right.Reset();
				attack_diagonal_up_left.Reset();
				attack_diagonal_down_right.Reset();
				attack_diagonal_down_left.Reset();

				if (animation == &attack_lateral_right || animation == &attack_lateral_left) animation = &idle_lateral;
				else if (animation == &attack_up) animation = &idle_up;
				else if (animation == &attack_down) animation = &idle_down;
				else if (animation == &attack_diagonal_up_right || animation == &attack_diagonal_up_left) animation = &idle_diagonal_up;
				else if (animation == &attack_diagonal_down_right || animation == &attack_diagonal_down_left) animation = &idle_diagonal_down;

				attacking = false;
			}
			else if (attackCollider != nullptr) {

				if (animation == &lateral || animation == &idle_lateral) {
					if (facingRight) attackCollider->SetPos((int)position.x + rightAttackSpawnPos, (int)position.y + 12);
					else attackCollider->SetPos((int)position.x + leftAttackSpawnPos, (int)position.y + 12);
				}
				else if (animation == &up || animation == &idle_up)	attackCollider->SetPos((int)position.x + margin.x, (int)position.y + margin.y - attackCollider->rect.h + 5);
				else if (animation == &down || animation == &idle_down)	attackCollider->SetPos((int)position.x + margin.x, (int)position.y + margin.y - 2);
				else if (animation == &diagonal_up || animation == &idle_diagonal_up) {
					if (facingRight) attackCollider->SetPos((int)position.x + rightAttackSpawnPos, (int)position.y + margin.y - attackCollider->rect.h + 5);
					else attackCollider->SetPos((int)position.x + leftAttackSpawnPos, (int)position.y + margin.y - attackCollider->rect.h + 5);
				}
				else if (animation == &diagonal_down || animation == &idle_diagonal_down) {
					if (facingRight) attackCollider->SetPos((int)position.x + rightAttackSpawnPos, (int)position.y + margin.y - 5);
					else attackCollider->SetPos((int)position.x + leftAttackSpawnPos, (int)position.y + margin.y - 5);
				}
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

		animation = &death;
		death.loop = false;
		App->fade->FadeToBlack();

		// Restarting the level in case the player dies
		if (App->fade->IsFading() == 0)
		{
			facingRight = true;
			position = { 200,750 };
			lifePoints = 200;

			App->entity->DestroyEntities();

			// Resetting the animation
			death.Reset();
			animation = &idle_diagonal_up;

			dead = false;
		}
	}

	// Checking for the heights
	App->map->EntityMovementTest(App->entity->knight);

	// Update collider position to player position
	if (collider != nullptr)
		collider->SetPos(position.x + margin.x, position.y + margin.y);

	return true;
}

// Call modules after each loop iteration
bool j1DragoonKnight::PostUpdate() {

	BROFILER_CATEGORY("DragoonKnightPostUpdate", Profiler::Color::Yellow)

	dialog->Update(0);

	if (App->scene1->finishedDialog)
		hud->Update(0);

	return true;
}

bool j1DragoonKnight::DrawOrder(float dt) {
	
	// ---------------------------------------------------------------------------------------------------------------------
	// DRAWING EVERYTHING ON THE SCREEN
	// ---------------------------------------------------------------------------------------------------------------------	
	SDL_Rect* r = &animation->GetCurrentFrame(dt);

	if (!attacking) {
		if (facingRight || animation == &up || animation == &down || animation == &idle_up || animation == &idle_down)
			if (animation == &down || animation == &idle_down) Draw(r, false, 0, 3, playerScale, offset, active_E);
			else Draw(r, false, 0, 0, playerScale, offset, active_E);
		else
			 Draw(r, true, -7, 0, playerScale, offset, active_E);
	}
	else {
		if (facingRight || animation == &attack_up || animation == &attack_down) {
			if (animation == &attack_lateral_right) Draw(r, false, 0, -6, playerScale, offset, active_E);
			else if (animation == &attack_up) Draw(r, false, 0, 0, playerScale, offset, active_E);
			else if (animation == &attack_down) Draw(r, false, -4, -1, playerScale, offset, active_E);
			else if (animation == &attack_diagonal_down_right) Draw(r, false, 0, -6, playerScale, offset, active_E);
			else Draw(r, false, 0, attackBlittingY, playerScale, offset, active_E);
		}
		else {
			if (animation == &attack_lateral_left) Draw(r, false, -13, -6, playerScale, offset, active_E);
			else if (animation == &attack_diagonal_down_left) Draw(r, false, -11, -6, playerScale, offset, active_E);
			else if (animation == &attack_diagonal_up_left) Draw(r, false, -11, -2, playerScale, offset, active_E);
			else Draw(r, true, attackBlittingX - 7, attackBlittingY, playerScale, offset, active_E);
		}
	}
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
	player = config.child("player").child("dragoonKnight");

	// Copying the size of the player
	playerSize.x = player.child("size").attribute("width").as_int();
	playerSize.y = player.child("size").attribute("height").as_int();
	attackSize.x = player.child("attackCollider").attribute("width").as_int();
	attackSize.y = player.child("attackCollider").attribute("height").as_int();
	margin.x = player.child("margin").attribute("x").as_int();
	margin.y = player.child("margin").attribute("y").as_int();
	offset = player.child("margin").attribute("offset").as_int();
	playerScale = player.attribute("scale").as_float();

	// Copying attack values
	attackBlittingX = player.child("attack").attribute("blittingX").as_int();
	attackBlittingY = player.child("attack").attribute("blittingY").as_int();
	rightAttackSpawnPos = player.child("attack").attribute("rightColliderSpawnPos").as_int();
	leftAttackSpawnPos = player.child("attack").attribute("leftColliderSpawnPos").as_int();

	// Copying combat values
	pugi::xml_node combat = player.child("combat");
	pugi::xml_node cd = player.child("cooldowns");

	basicDamage = combat.attribute("basicDamage").as_int();
	rageDamage = combat.attribute("rageDamage").as_uint();
	dashSpeed = combat.attribute("dashSpeed").as_int();
	lifePoints = combat.attribute("lifePoints").as_int();
	knockback = combat.attribute("knockback").as_int();
	totalLifePoints = combat.attribute("lifePoints").as_int();
	cooldownTime_Q = cd.attribute("Q").as_uint();
	cooldownTime_E = cd.attribute("E").as_uint();
	duration_Rage = cd.attribute("rage").as_uint();
	invulTime = cd.attribute("invulTime").as_uint();

	// Copying values of the speed
	pugi::xml_node speed = player.child("speed");
	movementSpeed = speed.child("movement").attribute("horizontal").as_float();
	godModeSpeed = speed.child("movement").attribute("godmode").as_float();
}