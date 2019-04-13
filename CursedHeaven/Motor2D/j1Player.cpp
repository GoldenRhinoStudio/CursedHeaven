#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1Input.h"
#include "j1Player.h"
#include "j1Render.h"
#include "j1FadeToBlack.h"
#include "j1BlackMage.h"
#include "j1Box.h"
#include "j1Map.h"
#include "j1Scene1.h"
#include "j1Window.h"

j1Player::j1Player(int x, int y, ENTITY_TYPES type) : j1Entity(x, y, ENTITY_TYPES::PLAYER) {}

j1Player::~j1Player() {}

void j1Player::UpdateCameraPosition(float dt)
{
	if (!changing_room) {
		App->render->camera.x = -position.x * App->win->GetScale() + (App->win->width / 2);
		App->render->camera.y = -position.y * App->win->GetScale() + (App->win->height / 2);
	}

	if (changing_room) {
		if (App->render->camera.x < -position.x * App->win->GetScale() + (App->win->width / 2))
			App->render->camera.x += 500 * dt;

		else if (App->render->camera.x > -position.x * App->win->GetScale() + (App->win->width / 2))
			App->render->camera.x -= 500 * dt;

		else if (App->render->camera.x - 20 < -position.x * App->win->GetScale() + (App->win->width / 2) ||
			App->render->camera.x + 20 > -position.x * App->win->GetScale() + (App->win->width / 2)
			)
			changing_room = false;

		else changing_room = false;

		if (App->render->camera.y < -position.y * App->win->GetScale() + (App->win->height / 2))
			App->render->camera.y += 250 * dt;

		else if (App->render->camera.y > -position.y * App->win->GetScale() + (App->win->height / 2))
			App->render->camera.y -= 250 * dt;

		else if (App->render->camera.y - 10 < -position.y * App->win->GetScale() + (App->win->height / 2) ||
			App->render->camera.y + 10 > -position.y * App->win->GetScale() + (App->win->height / 2)
			)
			changing_room = false;

		else changing_room = false;
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == j1KeyState::KEY_REPEAT)
		App->render->camera.x -= 20;
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == j1KeyState::KEY_REPEAT)
		App->render->camera.x += 20;
	if (App->input->GetKey(SDL_SCANCODE_UP) == j1KeyState::KEY_REPEAT)
		App->render->camera.y += 20;
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == j1KeyState::KEY_REPEAT)
		App->render->camera.y -= 20;
}

void j1Player::ManagePlayerMovement(DIRECTION& direction, float dt, bool do_logic, float speed) {

	if (do_logic)
		ChangeRoom(position.x, position.y);

	if (!changing_room && !App->gamePaused) {

		// GodMode controls
		if (GodMode) {
			if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT && (App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_IDLE && App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_IDLE))
			{
				position.x += godModeSpeed * dt;
				facingRight = true;
				direction = DIRECTION::RIGHT_;
			}

			if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_REPEAT)
			{
				position.x += godModeSpeed * dt;
				position.y -= (godModeSpeed / 2) * dt;
				facingRight = true;
				direction = DIRECTION::UP_RIGHT_;
			}

			if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_REPEAT)
			{
				position.x += godModeSpeed * dt;
				position.y += (godModeSpeed / 2) * dt;
				facingRight = true;
				direction = DIRECTION::DOWN_RIGHT_;
			}

			if (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT && (App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_IDLE && App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_IDLE))
			{
				position.x -= godModeSpeed * dt;
				facingRight = false;
				direction = DIRECTION::LEFT_;
			}

			if (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_REPEAT)
			{
				position.x -= godModeSpeed * dt;
				position.y -= (godModeSpeed / 2) * dt;
				facingRight = true;
				direction = DIRECTION::UP_LEFT_;
			}

			if (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_REPEAT)
			{
				position.x -= godModeSpeed * dt;
				position.y += (godModeSpeed / 2) * dt;
				facingRight = true;
				direction = DIRECTION::DOWN_LEFT_;
			}

			if (App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_REPEAT && (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_IDLE && App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_IDLE))
			{
				position.y -= godModeSpeed * dt;
				direction = DIRECTION::UP_;
			}


			if (App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_REPEAT && (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_IDLE && App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_IDLE))
			{
				position.y += godModeSpeed * dt;
				direction = DIRECTION::DOWN_;
			}

			// Idle
			if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_IDLE && App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_IDLE
				&& App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_IDLE && App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_IDLE
				&& attacking == false) {
				direction = DIRECTION::NONE_;
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
				position.x += speed * dt;
				facingRight = true;
				direction = DIRECTION::RIGHT_;
			}

			if ((App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisX < -6400) && CheckWalkability(left)) {
				position.x -= speed * dt;
				facingRight = false;
				direction = DIRECTION::LEFT_;
			}

			if (App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisY < -6400) {
				if (((App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisX < -6400) && CheckWalkability(up_left))
					|| ((App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisX > 6400) && CheckWalkability(up_right))) {

					if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisX > 6400) direction = DIRECTION::UP_RIGHT_;
					else direction = DIRECTION::UP_LEFT_;

					position.y -= (speed * dt) / 2;
				}
				else
					if (CheckWalkability(up)) {

						position.y -= speed * dt;
						direction = DIRECTION::UP_;
					}
			}

			if (App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisY > 6400) {
				if (((App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisX < -6400) && CheckWalkability(down_left))
					|| ((App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisX > 6400) && CheckWalkability(down_right))) {

					if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisX > 6400) direction = DIRECTION::DOWN_RIGHT_;
					else direction = DIRECTION::DOWN_LEFT_;

					position.y += (speed * dt) / 2;
				}
				else if (CheckWalkability(down)) {

					position.y += speed * dt;
					direction = DIRECTION::DOWN_;
				}
			}
		}
	}
}

void j1Player::SetMovementAnimations(DIRECTION& direction, Animation* idle_up, Animation* idle_down, Animation* idle_diagonal_up, Animation* idle_diagonal_down, Animation* idle_lateral,
	Animation* diagonal_up, Animation* diagonal_down, Animation* lateral, Animation* go_up, Animation* go_down) {

	if (direction == UP_LEFT_ || direction == UP_RIGHT_) animation = diagonal_up;
	else if (direction == DOWN_LEFT_ || direction == DOWN_RIGHT_) animation = diagonal_down;
	else if (direction == RIGHT_ || direction == LEFT_) animation = lateral;
	else if (direction == DOWN_) animation = go_down;
	else if (direction == UP_) animation = go_up;

	if ((App->entity->mage != nullptr && App->entity->mage->active_Q)
		|| ((App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_IDLE
		&& App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_IDLE
		&& App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_IDLE
		&& App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_IDLE)
		&& (App->input->gamepadLAxisY < 6400 && App->input->gamepadLAxisY > -6400
		&& App->input->gamepadLAxisX < 6400 && App->input->gamepadLAxisX > -6400))){

		if (animation == go_up) animation = idle_up;
		else if (animation == go_down) animation = idle_down;
		else if (animation == diagonal_down) animation = idle_diagonal_down;
		else if (animation == diagonal_up) animation = idle_diagonal_up;
		else if (animation == lateral) animation = idle_lateral;

		// Direction controls	
		if (App->input->gamepadRAxisX > 6400) {
			animation = idle_lateral;
			facingRight = true;
		}
		if (App->input->gamepadRAxisX < -6400) {
			animation = idle_lateral;
			facingRight = false;
		}
		if (App->input->gamepadRAxisY < -6400) {
			if ((App->input->gamepadRAxisX < -6400)	|| (App->input->gamepadRAxisX > 6400)) animation = idle_diagonal_up;
			else animation = idle_up;
		}
		if (App->input->gamepadRAxisY > 6400) {
			if ((App->input->gamepadRAxisX < -6400) || (App->input->gamepadRAxisX > 6400)) animation = idle_diagonal_down;
			else animation = idle_down;			
		}

		direction = DIRECTION::NONE_;
	}
}

void j1Player::ChangeRoom(int x, int y) {
	// 8,71 to 8,61
	if (App->map->WorldToMap((int)x, (int)y).x == 8 && App->map->WorldToMap((int)x, (int)y).y == 71)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(8, 61).x;
		position.y = App->map->MapToWorld(8, 61).y;
	}
	// 8,62 to 8,72
	if (App->map->WorldToMap((int)x, (int)y).x == 8 && App->map->WorldToMap((int)x, (int)y).y == 62)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(8, 72).x;
		position.y = App->map->MapToWorld(8, 72).y;
	}
	// 19,60 to 23,60
	if (App->map->WorldToMap((int)x, (int)y).x == 19 && App->map->WorldToMap((int)x, (int)y).y == 60)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(23, 60).x;
		position.y = App->map->MapToWorld(23, 60).y;
	}
	// 22,60 to 18,60
	if (App->map->WorldToMap((int)x, (int)y).x == 22 && App->map->WorldToMap((int)x, (int)y).y == 60)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(18, 60).x;
		position.y = App->map->MapToWorld(18, 60).y;
	}
	// 13,50 to 13,46
	if (App->map->WorldToMap((int)x, (int)y).x == 13 && App->map->WorldToMap((int)x, (int)y).y == 49)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(13, 46).x;
		position.y = App->map->MapToWorld(13, 46).y;
	}
	// 13,45 to 13,49
	if (App->map->WorldToMap((int)x, (int)y).x == 13 && App->map->WorldToMap((int)x, (int)y).y == 45)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(13, 50).x;
		position.y = App->map->MapToWorld(13, 50).y;
	}
	// 20,36 to 20,25
	if (App->map->WorldToMap((int)x, (int)y).x == 20 && App->map->WorldToMap((int)x, (int)y).y == 36)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(20, 25).x;
		position.y = App->map->MapToWorld(20, 25).y;
	}
	// 20,26 to 20,37
	if (App->map->WorldToMap((int)x, (int)y).x == 20 && App->map->WorldToMap((int)x, (int)y).y == 26)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(20, 37).x;
		position.y = App->map->MapToWorld(20, 37).y;
	}
	// 29,49 to 29,46
	if (App->map->WorldToMap((int)x, (int)y).x == 29 && App->map->WorldToMap((int)x, (int)y).y == 49)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(29, 46).x;
		position.y = App->map->MapToWorld(29, 46).y;
	}
	// 29,47 to 29,50
	if (App->map->WorldToMap((int)x, (int)y).x == 29 && App->map->WorldToMap((int)x, (int)y).y == 47)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(29, 50).x;
		position.y = App->map->MapToWorld(29, 50).y;
	}
	// 33,42 to 36,42
	if (App->map->WorldToMap((int)x, (int)y).x == 33 && App->map->WorldToMap((int)x, (int)y).y == 42)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(36, 42).x;
		position.y = App->map->MapToWorld(36, 42).y;
	}
	// 35,42 to 32,42
	if (App->map->WorldToMap((int)x, (int)y).x == 35 && App->map->WorldToMap((int)x, (int)y).y == 42)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(32, 42).x;
		position.y = App->map->MapToWorld(32, 42).y;
	}
	// 46,42 to 49,42
	if (App->map->WorldToMap((int)x, (int)y).x == 46 && App->map->WorldToMap((int)x, (int)y).y == 42)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(49, 42).x;
		position.y = App->map->MapToWorld(49, 42).y;
	}
	// 48,42 to 45,42
	if (App->map->WorldToMap((int)x, (int)y).x == 48 && App->map->WorldToMap((int)x, (int)y).y == 42)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(45, 42).x;
		position.y = App->map->MapToWorld(45, 42).y;
	}
	// 35,36 to 35,33
	if (App->map->WorldToMap((int)x, (int)y).x == 36 && App->map->WorldToMap((int)x, (int)y).y == 36)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(36, 33).x;
		position.y = App->map->MapToWorld(36, 33).y;
	}
	// 35,34 to 35,37
	if (App->map->WorldToMap((int)x, (int)y).x == 36 && App->map->WorldToMap((int)x, (int)y).y == 34)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(36, 37).x;
		position.y = App->map->MapToWorld(36, 37).y;
	}
	// 69,13 to 69,17
	if (App->map->WorldToMap((int)x, (int)y).x == 69 && App->map->WorldToMap((int)x, (int)y).y == 13)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(69, 17).x;
		position.y = App->map->MapToWorld(69, 17).y;
	}
	// 69,16 to 69,12
	if (App->map->WorldToMap((int)x, (int)y).x == 69 && App->map->WorldToMap((int)x, (int)y).y == 16)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(69, 14).x;
		position.y = App->map->MapToWorld(69, 14).y;
	}
	// 84,29 to 84,38
	if (App->map->WorldToMap((int)x, (int)y).x == 84 && App->map->WorldToMap((int)x, (int)y).y == 29)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(84, 38).x;
		position.y = App->map->MapToWorld(84, 38).y;
	}
	// 84,37 to 84,28
	if (App->map->WorldToMap((int)x, (int)y).x == 84 && App->map->WorldToMap((int)x, (int)y).y == 37)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(84, 28).x;
		position.y = App->map->MapToWorld(84, 28).y;
	}
	// 75,70 to 75,41
	if (App->map->WorldToMap((int)x, (int)y).x == 75 && App->map->WorldToMap((int)x, (int)y).y == 70)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(75, 41).x;
		position.y = App->map->MapToWorld(75, 41).y;
	}
	// 75,41 to 69,41
	if (App->map->WorldToMap((int)x, (int)y).x == 75 && App->map->WorldToMap((int)x, (int)y).y == 41)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(69, 41).x;
		position.y = App->map->MapToWorld(69, 41).y;
	}
	// 33,59 to 36,59
	if (App->map->WorldToMap((int)x, (int)y).x == 33 && App->map->WorldToMap((int)x, (int)y).y == 59)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(36, 59).x;
		position.y = App->map->MapToWorld(36, 59).y;
	}
	// 35,59 to 32,59
	if (App->map->WorldToMap((int)x, (int)y).x == 35 && App->map->WorldToMap((int)x, (int)y).y == 59)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(32, 59).x;
		position.y = App->map->MapToWorld(32, 59).y;
	}
	// 68,59 to 76,59
	if (App->map->WorldToMap((int)x, (int)y).x == 68 && App->map->WorldToMap((int)x, (int)y).y == 59)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(76, 59).x;
		position.y = App->map->MapToWorld(76, 59).y;
	}
	// 75,59 to 69,59
	if (App->map->WorldToMap((int)x, (int)y).x == 75 && App->map->WorldToMap((int)x, (int)y).y == 59)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(69, 59).x;
		position.y = App->map->MapToWorld(69, 59).y;
	}
	// 20,16 to 20,12
	if (App->map->WorldToMap((int)x, (int)y).x == 20 && App->map->WorldToMap((int)x, (int)y).y == 16)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(20, 12).x;
		position.y = App->map->MapToWorld(20, 12).y;
	}
	// 20,13 to 20,17
	if (App->map->WorldToMap((int)x, (int)y).x == 19 && App->map->WorldToMap((int)x, (int)y).y == 13)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(19, 17).x;
		position.y = App->map->MapToWorld(19, 17).y;
	}
	// 34,19 to 31,19
	if (App->map->WorldToMap((int)x, (int)y).x == 34 && App->map->WorldToMap((int)x, (int)y).y == 19)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(31, 19).x;
		position.y = App->map->MapToWorld(31, 19).y;
	}
	// 32,19 to 35,19
	if (App->map->WorldToMap((int)x, (int)y).x == 32 && App->map->WorldToMap((int)x, (int)y).y == 19)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(35, 19).x;
		position.y = App->map->MapToWorld(35, 19).y;
	}
	// 42,17 to 43,12
	if (App->map->WorldToMap((int)x, (int)y).x == 42 && App->map->WorldToMap((int)x, (int)y).y == 17)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(43, 12).x;
		position.y = App->map->MapToWorld(43, 12).y;
	}
	// 43,13 to 43,18
	if (App->map->WorldToMap((int)x, (int)y).x == 43 && App->map->WorldToMap((int)x, (int)y).y == 13)
	{
		changing_room = true;
		position.x = App->map->MapToWorld(43, 18).x;
		position.y = App->map->MapToWorld(43, 18).y;
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
			/*if (App->scene1->active)
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
				App->scene1->backToMenu = true;*/
		}
	}
};

bool j1Player::CheckWalkability(iPoint pos) const {

	/*bool ret = false;

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

	return ret;*/
	return true;
}