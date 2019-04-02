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

	// ---------------------------------------------------------------------------------------------------------------------
	// MANAGEMENT OF THE PLAYER
	// ---------------------------------------------------------------------------------------------------------------------

	if (player_start)
	{
		if (!attacking) 
			ManagePlayerMovement(App->entity->knight, dt, &idle_up, &idle_down, &idle_diagonal_up, &idle_diagonal_down, &idle_lateral,
				&diagonal_up, &diagonal_down, &lateral, &up, &down);

		// Attack control
		if ((App->input->GetKey(SDL_SCANCODE_P) == j1KeyState::KEY_DOWN || (SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_X)) == KEY_DOWN)
			&& attacking == false && GodMode == false && dead == false) {
			attacking = true;
			attackCollider->type = COLLIDER_ATTACK;

			//if (facingRight) {
				animation = &attack_lateral;
			/*}
			else {
				animation = &attackLeft;
			}*/
		}

		// Attack management
		if (attack_lateral.Finished() || dead == true) {

			attackCollider->type = COLLIDER_NONE;

			attack_lateral.Reset();
			//animation = &idle;
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
			playedSound = false;

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
		if (facingRight)
			Draw(r);
		else
			Draw(r, true);
	}
	else if (animation == &attack_lateral /*|| animation == &attackRight*/) {
		if (facingRight)
			Draw(r, false, 0, attackBlittingY);
		else
			Draw(r, false, attackBlittingX, attackBlittingY);
	}

	hud->Update(dt);

	// We update the camera to followe the player every frame
	UpdateCameraPosition();

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

	// Copying values of the speed
	pugi::xml_node speed = player.child("speed");

	horizontalSpeed = speed.child("movement").attribute("horizontal").as_float();
	godModeSpeed = speed.child("movement").attribute("godmode").as_float();
}