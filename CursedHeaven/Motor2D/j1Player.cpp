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
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == j1KeyState::KEY_REPEAT)
		App->render->camera.x -= 20;
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == j1KeyState::KEY_REPEAT)
		App->render->camera.x += 20;
	if (App->input->GetKey(SDL_SCANCODE_UP) == j1KeyState::KEY_REPEAT)
		App->render->camera.y += 20;
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == j1KeyState::KEY_REPEAT)
		App->render->camera.y -= 20;
}

void j1Player::ManagePlayerMovement(j1Player* currentPlayer, float dt, Animation* idle, Animation* diagonal_up, Animation* diagonal_down, Animation* lateral, Animation* go_up, Animation* go_down) {

	// GodMode controls
	if (GodMode) {
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
				animation = lateral;
				facingRight = true;
			}
			else
				animation = idle;
		}

		if (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT &&
			CheckWalkability(left)) {
			if (dead == false) {
				position.x -= horizontalSpeed * dt;
				animation = lateral;
				facingRight = false;
			}
			else
				animation = idle;
		}

		if (App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_REPEAT) {
			if (dead == false) {
				if ((App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT && CheckWalkability(up_left))
					|| (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT && CheckWalkability(up_right))) {
					
					animation = diagonal_up;
					position.y -= (horizontalSpeed * dt) / 2;
				}
				else if (CheckWalkability(up) && (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_IDLE)
					&& (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_IDLE)){

					position.y -= horizontalSpeed * dt;
					animation = go_up;
				}
			}
			else
				animation = idle;
		}

		if (App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_REPEAT) {
			if (dead == false) {
				if ((App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT && CheckWalkability(down_left))
					|| (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT && CheckWalkability(down_right))) {

					animation = diagonal_down;
					position.y += (horizontalSpeed * dt) / 2;
				}
				else if (CheckWalkability(down)) {

					position.y += horizontalSpeed * dt;
					animation = go_down;
				}
			}
			else
				animation = idle;
		}

		if ((App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT
			&& App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT)
			|| (App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_REPEAT
				&& App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_REPEAT))
			animation = idle;
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
		if (App->entity->current_height == 0) {
			if (App->map->data.layers.begin()._Ptr->_Next->_Next->_Next->_Myval->Get(pos.x, pos.y) == 0)
				ret = true;
		}
		else if (App->entity->current_height == 1) {
			if (App->map->data.layers.begin()._Ptr->_Next->_Next->_Next->_Next->_Myval->Get(pos.x, pos.y) == 0)
				ret = true;
		}
		else if (App->entity->current_height == 2) {
			if (App->map->data.layers.begin()._Ptr->_Next->_Next->_Next->_Next->_Next->_Myval->Get(pos.x, pos.y) == 0)
				ret = true;
		}		
	}

	return ret;
}