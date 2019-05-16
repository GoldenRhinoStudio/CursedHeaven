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
#include "j1Shop.h"
#include "j1Box.h"
#include "j1Map.h"
#include "j1Scene1.h"
#include "j1Window.h"
#include "j1Audio.h"

j1Player::j1Player(int x, int y, ENTITY_TYPES type) : j1Entity(x, y, ENTITY_TYPES::PLAYER) {}

j1Player::~j1Player() {}

void j1Player::UpdateCameraPosition(float dt)
{/*
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
	*/
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == j1KeyState::KEY_REPEAT)
		App->render->camera.x -= 30;
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == j1KeyState::KEY_REPEAT)
		App->render->camera.x += 30;
	if (App->input->GetKey(SDL_SCANCODE_UP) == j1KeyState::KEY_REPEAT)
		App->render->camera.y += 30;
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == j1KeyState::KEY_REPEAT)
		App->render->camera.y -= 30;
}

void j1Player::ManagePlayerMovement(DIRECTION& direction, float dt, bool do_logic, float speed) {

	/*if (do_logic)
		ChangeRoom(position.x, position.y);*/

	if (!changing_room && !App->gamePaused && !App->scene1->profile_active && !dead) {

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

			if (App->shop->potions > 0 && (App->input->GetKey(SDL_SCANCODE_R) == j1KeyState::KEY_DOWN || SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_X) == KEY_DOWN)) {
				App->shop->potions--;
				App->entity->currentPlayer->lifePoints += App->shop->potionHealing;
			}

			// Direction controls	
			if ((App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisX > 6400)) {
				if (movement)
					position.x += speed * dt;
				facingRight = true;
				direction = DIRECTION::RIGHT_;
			}

			if ((App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisX < -6400)) {
				if (movement)
					position.x -= speed * dt;
				facingRight = false;
				direction = DIRECTION::LEFT_;
			}

			if (App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisY < -6400) {
				if ((App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisX < -6400)
					|| (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisX > 6400)) {

					if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisX > 6400) direction = DIRECTION::UP_RIGHT_;
					else direction = DIRECTION::UP_LEFT_;
					if (movement)
						position.y -= (speed * dt) / 2;
				}
				else if (movement){
					position.y -= speed * dt;
					direction = DIRECTION::UP_;
				}
			}

			if (App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisY > 6400) {
				if ((App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisX < -6400)
					|| (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisX > 6400)) {

					if (App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_REPEAT || App->input->gamepadLAxisX > 6400) direction = DIRECTION::DOWN_RIGHT_;
					else direction = DIRECTION::DOWN_LEFT_;
					if (movement)
						position.y += (speed * dt) / 2;
				}
				else if (movement){
						position.y += speed * dt;
					direction = DIRECTION::DOWN_;
				}
			}
		}
	}
}

void j1Player::SetMovementAnimations(DIRECTION& direction, Animation* idle_up, Animation* idle_down, Animation* idle_diagonal_up, Animation* idle_diagonal_down, Animation* idle_lateral,
	Animation* diagonal_up, Animation* diagonal_down, Animation* lateral, Animation* go_up, Animation* go_down, Animation* death) {

	if (dead) animation = death;

	if (direction == UP_LEFT_ || direction == UP_RIGHT_) animation = diagonal_up;
	else if (direction == DOWN_LEFT_ || direction == DOWN_RIGHT_) animation = diagonal_down;
	else if (direction == RIGHT_ || direction == LEFT_) animation = lateral;
	else if (direction == DOWN_) animation = go_down;
	else if (direction == UP_) animation = go_up;

	if (direction == UP_LEFT_ || direction == LEFT_ || direction == DOWN_LEFT_)
		animation->flip = true;
	else 
		animation->flip = false;

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
	
	// 8,71 to 8,61 1-2
	if ((App->map->WorldToMap((int)x, (int)y).x == 10 || App->map->WorldToMap((int)x, (int)y).x == 11 || App->map->WorldToMap((int)x, (int)y).x == 12)
		&& App->map->WorldToMap((int)x, (int)y).y == 68)
	{
		
	}

	// 19,60 to 23,60 2-3
	if (App->map->WorldToMap((int)x, (int)y).x == 23 && 
		(App->map->WorldToMap((int)x, (int)y).y == 62 || App->map->WorldToMap((int)x, (int)y).y == 61 || App->map->WorldToMap((int)x, (int)y).y == 63))
	{
		
	}
	
	// 13,50 to 13,46 2-4
	if ((App->map->WorldToMap((int)x, (int)y).x == 15 || App->map->WorldToMap((int)x, (int)y).x == 16 || App->map->WorldToMap((int)x, (int)y).x == 17)
		&& App->map->WorldToMap((int)x, (int)y).y == 49)
	{
		
	}

	// 20,36 to 20,25 4-7
	if ((App->map->WorldToMap((int)x, (int)y).x == 22 || App->map->WorldToMap((int)x, (int)y).x == 23 || App->map->WorldToMap((int)x, (int)y).x == 24)
		&& App->map->WorldToMap((int)x, (int)y).y == 34)
	{
		
	}

	// 29,49 to 29,46 3-4
	if ((App->map->WorldToMap((int)x, (int)y).x == 31 || App->map->WorldToMap((int)x, (int)y).x == 32 || App->map->WorldToMap((int)x, (int)y).x == 33)
		&& App->map->WorldToMap((int)x, (int)y).y == 50)
	{
		
	}
	
	// 33,42 to 36,42 4-5
	if (App->map->WorldToMap((int)x, (int)y).x == 36 &&
		(App->map->WorldToMap((int)x, (int)y).y == 44 || App->map->WorldToMap((int)x, (int)y).y == 45 || App->map->WorldToMap((int)x, (int)y).y == 43))
	{
		
	}
	
	// 46,42 to 49,42 5-6
	if (App->map->WorldToMap((int)x, (int)y).x == 49 &&
		(App->map->WorldToMap((int)x, (int)y).y == 44 || App->map->WorldToMap((int)x, (int)y).y == 45 || App->map->WorldToMap((int)x, (int)y).y == 43))
	{
		
	}
	
	// 35,36 to 35,33 5-8
	if ((App->map->WorldToMap((int)x, (int)y).x == 38 || App->map->WorldToMap((int)x, (int)y).x == 39 || App->map->WorldToMap((int)x, (int)y).x == 37)
		&& App->map->WorldToMap((int)x, (int)y).y == 37)
	{
		
	}
	
	// 69,13 to 69,17 10-11
	if ((App->map->WorldToMap((int)x, (int)y).x == 71 || App->map->WorldToMap((int)x, (int)y).x == 72 || App->map->WorldToMap((int)x, (int)y).x == 70)
		&& App->map->WorldToMap((int)x, (int)y).y == 16)
	{
		
	}
	
	// 84,29 to 84,38 11-12
	if ((App->map->WorldToMap((int)x, (int)y).x == 86 || App->map->WorldToMap((int)x, (int)y).x == 87 || App->map->WorldToMap((int)x, (int)y).x == 85)
		&& App->map->WorldToMap((int)x, (int)y).y == 35)
	{
		
	}
	
	// 75,70 to 75,41 6-12
	if (App->map->WorldToMap((int)x, (int)y).x == 75 &&
		(App->map->WorldToMap((int)x, (int)y).y == 42 || App->map->WorldToMap((int)x, (int)y).y == 43 || App->map->WorldToMap((int)x, (int)y).y == 44))
	{
		
	}
	
	// 33,59 to 36,59 3-13
	if (App->map->WorldToMap((int)x, (int)y).x == 36 &&
		(App->map->WorldToMap((int)x, (int)y).y == 61 || App->map->WorldToMap((int)x, (int)y).y == 59 || App->map->WorldToMap((int)x, (int)y).y == 60))
	{
		
	}
	
	// 68,59 to 76,59 13-12
	if (App->map->WorldToMap((int)x, (int)y).x == 74 && 
		(App->map->WorldToMap((int)x, (int)y).y == 62 || App->map->WorldToMap((int)x, (int)y).y == 61 || App->map->WorldToMap((int)x, (int)y).y == 60))
	{
		
	}
	
	// 20,16 to 20,12 7-9
	if ((App->map->WorldToMap((int)x, (int)y).x == 22 || App->map->WorldToMap((int)x, (int)y).x == 23 || App->map->WorldToMap((int)x, (int)y).x == 21)
		&& App->map->WorldToMap((int)x, (int)y).y == 16)
	{
		
	}
	
	// 34,19 to 31,19 8-7
	if (App->map->WorldToMap((int)x, (int)y).x == 35 && 
		(App->map->WorldToMap((int)x, (int)y).y == 21 || App->map->WorldToMap((int)x, (int)y).y == 22 || App->map->WorldToMap((int)x, (int)y).y == 20))
	{
		
	}
	
	// 42,17 to 43,12 8-10
	if ((App->map->WorldToMap((int)x, (int)y).x == 45 || App->map->WorldToMap((int)x, (int)y).x == 46 || App->map->WorldToMap((int)x, (int)y).x == 44)
		&& App->map->WorldToMap((int)x, (int)y).y == 16)
	{
		
	}
}

// Detects collisions
void j1Player::OnCollision(Collider* col_1, Collider* col_2)
{
	if (col_1->type == COLLIDER_PLAYER || col_1->type == COLLIDER_NONE)
	{
		//If the player collides with death colliders				
		if (invulCounter.Read() >= lastTime_invul + invulTime) {
			receivedDamage = false;
			lastTime_invul = invulCounter.Read();
		}

		if (!receivedDamage && col_2->type == COLLIDER_ENEMY)
		{
			lifePoints -= App->entity->slime_Damage;
			if (App->entity->player_type == MAGE) App->audio->PlayFx(App->audio->damage_bm);
			else App->audio->PlayFx(App->audio->damage_dk);
			receivedDamage = true;
			
			if (lifePoints <= 0) dead = true;
		}

		if (!receivedDamage && col_2->type == COLLIDER_ENEMY_SHOT)
		{			
			lifePoints -= App->entity->mindflyer_Damage;
			if (App->entity->player_type == MAGE) App->audio->PlayFx(App->audio->damage_bm);
			else App->audio->PlayFx(App->audio->damage_dk);
			receivedDamage = true;

			if (lifePoints <= 0) dead = true;
		}
	}
};