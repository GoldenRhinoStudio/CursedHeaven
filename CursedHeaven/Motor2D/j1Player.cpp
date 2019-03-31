#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1Input.h"
#include "j1Player.h"
#include "j1Render.h"
#include "j1FadeToBlack.h"
#include "j1Box.h"
#include "j1Map.h"
#include "j1Scene1.h"

j1Player::j1Player(int x, int y, ENTITY_TYPES type) : j1Entity(x, y, ENTITY_TYPES::PLAYER) {}

j1Player::~j1Player() {}

void j1Player::UpdateCameraPosition() 
{
	if (App->input->GetKey(SDL_SCANCODE_C) == j1KeyState::KEY_REPEAT) {
		App->render->camera.x = 0;
		App->render->camera.y = 0;
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == j1KeyState::KEY_REPEAT)
		App->render->camera.x -= 20;
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == j1KeyState::KEY_REPEAT)
		App->render->camera.x += 20;
	if (App->input->GetKey(SDL_SCANCODE_UP) == j1KeyState::KEY_REPEAT)
		App->render->camera.y += 20;
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == j1KeyState::KEY_REPEAT)
		App->render->camera.y -= 20;

	LoadPlayerProperties();

	animation = &idle;
	currentJumps = initialJumps;

	lives = 2;

	// Setting player position
	position.x = 400;
	position.y = 400;

	GodMode = true;
	if (GodMode)
		collider = App->collisions->AddCollider({ (int)position.x + margin.x, (int)position.y + margin.y, 10, 5 }, COLLIDER_NONE, App->entity); //CHECK
	else
		collider = App->collisions->AddCollider({ (int)position.x + margin.x, (int)position.y + margin.y, playerSize.x, playerSize.y }, COLLIDER_PLAYER, App->entity);

	//attackCollider = App->collisions->AddCollider({ (int)position.x + rightAttackSpawnPos, (int)position.y + margin.y, playerSize.x, playerSize.y }, COLLIDER_NONE, App->entity);

	hud = new j1Hud();
	hud->Start();

	player_start = true;
	return true;
}

//Call modules before each loop iteration
bool j1Player::PreUpdate() {

	BROFILER_CATEGORY("PlayerPreUpdate", Profiler::Color::Orange)

	return true;
}

void j1Player::ManagePlayerMovement(j1Player* currentPlayer, float dt, Animation* godmode, Animation* idle, Animation* run) {

	if (player_start)
	{
		ChangeRoom(position.x, position.y);

		if (!changing_room) {

			// GodMode controls
			if (GodMode) {

				animation = godmode;

				if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT && (App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_IDLE && App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_IDLE))
				{
					position.x += godModeSpeed * dt;
					facingRight = true;
					currentPlayer->direction = DIRECTION::RIGHT_;
				}

				if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_REPEAT)
				{
					position.x += godModeSpeed * dt;
					position.y -= (godModeSpeed / 2) * dt;
					facingRight = true;
					currentPlayer->direction = DIRECTION::UP_RIGHT_;
				}

				if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_REPEAT)
				{
					position.x += godModeSpeed * dt;
					position.y += (godModeSpeed / 2) * dt;
					facingRight = true;
					currentPlayer->direction = DIRECTION::DOWN_RIGHT_;
				}

				if (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT && (App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_IDLE && App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_IDLE))
				{
					position.x -= godModeSpeed * dt;
					facingRight = false;
					currentPlayer->direction = DIRECTION::LEFT_;
				}

				if (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_REPEAT)
				{
					position.x -= godModeSpeed * dt;
					position.y -= (godModeSpeed / 2) * dt;
					facingRight = true;
					currentPlayer->direction = DIRECTION::UP_LEFT_;
				}

				if (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_REPEAT)
				{
					position.x -= godModeSpeed * dt;
					position.y += (godModeSpeed / 2) * dt;
					facingRight = true;
					currentPlayer->direction = DIRECTION::DOWN_LEFT_;
				}

				if (App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_REPEAT && (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_IDLE && App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_IDLE))
				{
					position.y -= godModeSpeed * dt;
					currentPlayer->direction = DIRECTION::UP_;
				}


				if (App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_REPEAT && (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_IDLE && App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_IDLE))
				{
					position.y += godModeSpeed * dt;
					currentPlayer->direction = DIRECTION::DOWN_;
				}

				// Idle
				if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_IDLE && App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_IDLE
					&& App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_IDLE && App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_IDLE
					&& attacking == false) {
					currentPlayer->direction = DIRECTION::NONE_;
				}
			}

			else {
				// Position of the player in map coordinates
				iPoint mapPos = App->map->WorldToMap((int)position.x, (int)position.y);

				iPoint up_right = { mapPos.x, mapPos.y - 1 };
				iPoint down_left = { mapPos.x, mapPos.y + 1 };
				iPoint down_right = { mapPos.x + 1, mapPos.y };
				iPoint up_left = { mapPos.x - 1, mapPos.y };

				iPoint right = { mapPos.x + 1, mapPos.y - 1 };
				iPoint up = { mapPos.x - 1, mapPos.y - 1 };
				iPoint down = { mapPos.x + 1, mapPos.y + 1 };;
				iPoint left = { mapPos.x - 1, mapPos.y + 1 };

				// Idle
				if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_IDLE
					&& App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_IDLE
					&& App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_IDLE
					&& App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_IDLE) {
					animation = idle;
					currentPlayer->direction = DIRECTION::NONE_;
				}

				// Direction controls	
				if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT &&
					CheckWalkability(right)) {
					if (dead == false) {
						position.x += horizontalSpeed * dt;
						animation = run;
						facingRight = true;
					}
					else
						animation = idle;
				}

				if (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT &&
					CheckWalkability(left)) {
					if (dead == false) {
						position.x -= horizontalSpeed * dt;
						animation = run;
						facingRight = false;
					}
					else
						animation = idle;
				}

				if (App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_REPEAT) {
					if (dead == false) {
						if ((App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT && CheckWalkability(up_left))
							|| (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT && CheckWalkability(up_right)))
							position.y -= (horizontalSpeed * dt) / 2;

						else if (CheckWalkability(up) && (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_IDLE)
							&& (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_IDLE))
							position.y -= horizontalSpeed * dt;

						animation = run;
					}
					else
						animation = idle;
				}

				if (App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_REPEAT) {
					if (dead == false) {
						if ((App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT && CheckWalkability(down_left))
							|| (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT && CheckWalkability(down_right)))
							position.y += (horizontalSpeed * dt) / 2;
						else if (CheckWalkability(down))
							position.y += horizontalSpeed * dt;

						animation = run;
					}
					else
						animation = idle;
				}

				if ((App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT
					&& App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT)
					|| (App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_REPEAT
						&& App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_REPEAT))
					animation = idle;

				// God Mode
				if (App->input->GetKey(SDL_SCANCODE_F10) == j1KeyState::KEY_DOWN && dead == false)
				{
					GodMode = !GodMode;

					if (GodMode == true)
					{
						collider->type = COLLIDER_NONE;
						animation = godmode;

					}
					else if (GodMode == false)
					{
						collider->type = COLLIDER_PLAYER;
					}
				}
			}
		}
	}
}

// Detects collisions
void j1Player::OnCollision(Collider* col_1, Collider* col_2)
{
	if (col_1->type == COLLIDER_PLAYER || col_1->type == COLLIDER_NONE)
	{
		//If the player collides with win colliders
		if (col_2->type == COLLIDER_WIN)
		{
			App->fade->FadeToBlack();

			if (App->scene1->active)
				App->scene1->changingScene = true;
		}
		else
		//If the player collides with death colliders
		if (col_2->type == COLLIDER_ENEMY)
		{
			if (App->scene1->active)
				App->scene1->settings_window->position = App->gui->settingsPosition;

			App->fade->FadeToBlack(3.0f);

			if (dead)
			{
				App->entity->DestroyEntities();

				dead = true;
				points = 0;
				score_points = 0;
			}
			else if (App->scene1->active)
				App->scene1->backToMenu = true;
		}
	}
};

bool j1Player::CheckWalkability(iPoint pos) const {

	bool ret = false;

	if ((pos.x >= 0 && pos.x < App->map->data.width)
		&& (pos.y >= 0 && pos.y < App->map->data.height))
	{
		if (App->map->data.layers.begin()._Ptr->_Next->_Next->_Next->_Myval->Get(pos.x, pos.y) == 0)
			ret = true;
	}

	return ret;
}

void j1Player::ChangeRoom(int x, int y) {
	// Room 1 to 2 (42,28) to (42,17)
	if (App->map->WorldToMap((int)x, (int)y).x == 42 && App->map->WorldToMap((int)x, (int)y).y == 28)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(42, 17).x;
		position.y = App->map->MapToWorld(42, 17).y;
	}
	// Room 2 to 1 (42,18) to (42,29)
	if (App->map->WorldToMap((int)x, (int)y).x == 42 && App->map->WorldToMap((int)x, (int)y).y == 18) 
	{
		changing_room = true;
		position.x = App->map->MapToWorld(42, 29).x;
		position.y = App->map->MapToWorld(42, 29).y;
	}
	// Room 1 to 3 (38,32) to (20,32)
	if (App->map->WorldToMap((int)x, (int)y).x == 38 && App->map->WorldToMap((int)x, (int)y).y == 32)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(20, 32).x;
		position.y = App->map->MapToWorld(20, 32).y;
	}
	// Room 3 to 1 (21,32) to (39,32)
	if (App->map->WorldToMap((int)x, (int)y).x == 21 && App->map->WorldToMap((int)x, (int)y).y == 32)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(39, 32).x;
		position.y = App->map->MapToWorld(39, 32).y;
	}
	// Room 1 to 4 (46,32) to (72,32)
	if (App->map->WorldToMap((int)x, (int)y).x == 46 && App->map->WorldToMap((int)x, (int)y).y == 32)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(72, 32).x;
		position.y = App->map->MapToWorld(72, 32).y;
	}
	// Room 4 to 1 (71,32) to (45,32)
	if (App->map->WorldToMap((int)x, (int)y).x == 71 && App->map->WorldToMap((int)x, (int)y).y == 32)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(45, 32).x;
		position.y = App->map->MapToWorld(45, 32).y;
	}
	// Room 1 to 5 (42,36) to (42,48)
	if (App->map->WorldToMap((int)x, (int)y).x == 42 && App->map->WorldToMap((int)x, (int)y).y == 36)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(42, 48).x;
		position.y = App->map->MapToWorld(42, 48).y;
	}
	// Room 5 to 1 (42,47) to (42,35)
	if (App->map->WorldToMap((int)x, (int)y).x == 42 && App->map->WorldToMap((int)x, (int)y).y == 47)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(42, 35).x;
		position.y = App->map->MapToWorld(42, 35).y;
	}
	// Room 6 to 5 (42,77) to (42,60)
	if (App->map->WorldToMap((int)x, (int)y).x == 43 && App->map->WorldToMap((int)x, (int)y).y == 77)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(43, 60).x;
		position.y = App->map->MapToWorld(43, 60).y;
	}
	// Room 5 to 6 (42,61) to (42,78)
	if (App->map->WorldToMap((int)x, (int)y).x == 43 && App->map->WorldToMap((int)x, (int)y).y == 61)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(43, 78).x;
		position.y = App->map->MapToWorld(43, 78).y;
	}
}