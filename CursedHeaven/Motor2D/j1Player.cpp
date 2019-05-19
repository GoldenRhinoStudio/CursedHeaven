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

void j1Player::ManagePlayerMovement(DIRECTION& direction, float dt, bool do_logic, float speed) {

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

			if (App->shop->potions > 0 && (App->input->GetKey(SDL_SCANCODE_R) == j1KeyState::KEY_DOWN ||
				(SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_X) == KEY_DOWN && potionTime.Read() >= lastPotionTime + 500))) {
				App->shop->potions--;
				lastPotionTime = potionTime.Read();
				App->entity->currentPlayer->lifePoints += App->shop->potionHealing;
				App->audio->PlayFx(App->audio->potion_sound);
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
				else if (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_IDLE && App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_IDLE) {
					direction = DIRECTION::UP_;
					if (movement)
						position.y -= speed * dt;
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
				else if (App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_IDLE && App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_IDLE) {
					direction = DIRECTION::DOWN_;
					if (movement)
						position.y += speed * dt;
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
				&& App->input->gamepadLAxisX < 6400 && App->input->gamepadLAxisX > -6400))) {

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
			if ((App->input->gamepadRAxisX < -6400) || (App->input->gamepadRAxisX > 6400)) animation = idle_diagonal_up;
			else animation = idle_up;
		}
		if (App->input->gamepadRAxisY > 6400) {
			if ((App->input->gamepadRAxisX < -6400) || (App->input->gamepadRAxisX > 6400)) animation = idle_diagonal_down;
			else animation = idle_down;
		}

		direction = DIRECTION::NONE_;
	}
}

// Detects collisions
void j1Player::OnCollision(Collider* col_1, Collider* col_2)
{
	if (col_2->type == COLLIDER_ENEMY || col_2->type == COLLIDER_ENEMY_SHOT)
	{
		if (App->entity->player_type == KNIGHT && App->entity->currentPlayer->active_E) return;

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
	}
};