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

void j1Player::ManagePlayerMovement(j1Player* currentPlayer, float dt) {

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

		// Direction controls	
		if ((App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisX > 6400) && CheckWalkability(right)) {
			position.x += horizontalSpeed * dt;
			facingRight = true;
			currentPlayer->direction = DIRECTION::RIGHT_;			
		}

		if ((App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisX < -6400) && CheckWalkability(left)) {
			position.x -= horizontalSpeed * dt;
			facingRight = false;
			currentPlayer->direction = DIRECTION::LEFT_;
		}

		if (App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisY < -6400) {
			if (((App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisX < -6400) && CheckWalkability(up_left))
			|| ((App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisX > 6400) && CheckWalkability(up_right))) {

				if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT) currentPlayer->direction = DIRECTION::UP_RIGHT_;
				else currentPlayer->direction = DIRECTION::UP_LEFT_;

				position.y -= (horizontalSpeed * dt) / 2;
			}
			else 
			if (CheckWalkability(up) && (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_IDLE)
				&& (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_IDLE)){

				position.y -= horizontalSpeed * dt;
				currentPlayer->direction = DIRECTION::UP_;
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisY > 6400) {
			if (((App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisX < -6400) && CheckWalkability(down_left))
				|| ((App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisX > 6400) && CheckWalkability(down_right))) {
				
				if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT) currentPlayer->direction = DIRECTION::DOWN_RIGHT_;
				else currentPlayer->direction = DIRECTION::DOWN_LEFT_;

				position.y += (horizontalSpeed * dt) / 2;
			}
			else if (CheckWalkability(down)) {

				position.y += horizontalSpeed * dt;
				currentPlayer->direction = DIRECTION::DOWN_;
			}			
		}
	}	
}

void j1Player::SetMovementAnimations(Animation* idle_up, Animation* idle_down, Animation* idle_diagonal_up, Animation* idle_diagonal_down, Animation* idle_lateral,
	Animation* diagonal_up, Animation* diagonal_down, Animation* lateral, Animation* go_up, Animation* go_down) {


	if (direction == UP_LEFT_ || direction == UP_RIGHT_) animation = diagonal_up;
	else if (direction == DOWN_LEFT_ || direction == DOWN_RIGHT_) animation = diagonal_down;
	else if (direction == RIGHT_ || direction == LEFT_) animation = lateral;
	else if (direction == DOWN_) animation = go_down;
	else if (direction == UP_) animation = go_up;

	if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_IDLE
		&& App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_IDLE
		&& App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_IDLE
		&& App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_IDLE) {

		if (animation == go_up) animation = idle_up;
		else if (animation == go_down) animation = idle_down;
		else if (animation == diagonal_up) animation = idle_diagonal_up;
		else if (animation == diagonal_down) animation = idle_diagonal_down;
		else if (animation == lateral) animation = idle_lateral;

		direction = DIRECTION::NONE_;
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