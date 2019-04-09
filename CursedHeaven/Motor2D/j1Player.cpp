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
#include "j1Window.h"
#include "j1Particles.h"

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

	if ((App->input->GetKey(SDL_SCANCODE_D) == j1KeyState::KEY_IDLE
		&& App->input->GetKey(SDL_SCANCODE_A) == j1KeyState::KEY_IDLE
		&& App->input->GetKey(SDL_SCANCODE_W) == j1KeyState::KEY_IDLE
		&& App->input->GetKey(SDL_SCANCODE_S) == j1KeyState::KEY_IDLE)
		&& (App->input->gamepadLAxisY < 6400 && App->input->gamepadLAxisY > -6400
		&& App->input->gamepadLAxisX < 6400 && App->input->gamepadLAxisX > -6400)){

		if (animation == go_up) animation = idle_up;
		else if (animation == go_down) animation = idle_down;
		else if (animation == diagonal_up) animation = idle_diagonal_up;
		else if (animation == diagonal_down) animation = idle_diagonal_down;
		else if (animation == lateral) animation = idle_lateral;

		direction = DIRECTION::NONE_;
	}
}

void j1Player::Shot(float x, float y) {

	//to change where the particle is born (in this case: from the centre of the player aprox (+8,+8))
	fPoint margin;
	margin.x = 8;
	margin.y = 8;

	fPoint edge;
	edge.x = x - (position.x + margin.x) - (App->render->camera.x / (int)App->win->GetScale());
	edge.y = (position.y + margin.y) - y + (App->render->camera.y / (int)App->win->GetScale());

	//float distance = sqrt(((edge.x) ^ 2) + ((edge.y) ^ 2));
	//edge.NormalizeVector();

	//if the map is very big and its not enough accurate, we should use long double for the var angle
	double angle = -(atan2(edge.y, edge.x));

	//float angle = 1 / cos(edge.x / distance);

	fPoint speed_particle;

	App->particles->particle_speed = { 200,200 };

	speed_particle.x = App->particles->particle_speed.x * cos(angle);
	speed_particle.y = App->particles->particle_speed.y * sin(angle);

	App->particles->AddParticle(App->particles->shot_right, position.x + margin.x, position.y + margin.y, speed_particle, COLLIDER_SHOT);
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