#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1Input.h"
#include "j1BlackMage.h"
#include "j1Render.h"
#include "j1FadeToBlack.h"
#include "j1Audio.h"
#include "j1Hud.h"
#include "j1Map.h"
#include "j1Timer.h"

#include "Brofiler/Brofiler.h"

j1BlackMage::j1BlackMage(int x, int y, ENTITY_TYPES type) : j1Player(x, y, ENTITY_TYPES::PLAYER)
{
	animation = NULL;

	idle_diagonal_up.LoadAnimation("idleD_up", "mage");
	idle_diagonal_down.LoadAnimation("idleD_down", "mage");
	idle_lateral.LoadAnimation("idleLateral", "mage");
	idle_down.LoadAnimation("idleDown", "mage");
	idle_up.LoadAnimation("idleUp", "mage");

	up.LoadAnimation("up", "mage");
	down.LoadAnimation("down", "mage");
	lateral.LoadAnimation("lateral", "mage");
	diagonal_up.LoadAnimation("diagonalUp", "mage");
	diagonal_down.LoadAnimation("diagonalDown", "mage");
	godmode.LoadAnimation("godmode", "mage");
}

j1BlackMage::~j1BlackMage() {}

// Load assets
bool j1BlackMage::Start() {

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
bool j1BlackMage::PreUpdate() {

	BROFILER_CATEGORY("BlackMagePreUpdate", Profiler::Color::Orange)

	return true;
}

// Call modules on each loop iteration
bool j1BlackMage::Update(float dt, bool do_logic) {

	BROFILER_CATEGORY("BlackMageUpdate", Profiler::Color::LightSeaGreen)

	if (player_start)
	{
		if (!attacking && !active_Q) {
			ManagePlayerMovement(App->entity->mage, dt, do_logic);
			SetMovementAnimations(&idle_up, &idle_down, &idle_diagonal_up, &idle_diagonal_down, &idle_lateral,
				&diagonal_up, &diagonal_down, &lateral, &up, &down);
		}

		// ---------------------------------------------------------------------------------------------------------------------
		// COMBAT
		// ---------------------------------------------------------------------------------------------------------------------
		if (GodMode == false && dead == false && changing_room == false) {
			if (!attacking) {
				
			}


			if ((App->input->GetMouseButtonDown(1) == KEY_DOWN))
			{
				iPoint mouse_pos;
				App->input->GetMousePosition(mouse_pos.x, mouse_pos.y);
				Shot(mouse_pos.x, mouse_pos.y);
			}
			
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
			animation = &idle_diagonal_up;

			dead = false;
		}
	}

	// Checking for the heights
	App->map->EntityMovement(App->entity->mage);

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
		if (facingRight) {
			Draw(r, false, 0, attackBlittingY);
		}
		else {
			Draw(r, true, attackBlittingX, attackBlittingY);
		}
	}

	hud->Update(dt);

	// We update the camera to follow the player every frame
	UpdateCameraPosition(dt);

	return true;
}

// Call modules after each loop iteration
bool j1BlackMage::PostUpdate() {

	BROFILER_CATEGORY("BlackMagePostUpdate", Profiler::Color::Yellow)

	return true;
}

// Load game state
bool j1BlackMage::Load(pugi::xml_node& data) {

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
bool j1BlackMage::Save(pugi::xml_node& data) const {

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
bool j1BlackMage::CleanUp() {

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

void j1BlackMage::LoadPlayerProperties() {

	pugi::xml_document config_file;
	config_file.load_file("config.xml");
	pugi::xml_node config;
	config = config_file.child("config");
	pugi::xml_node player;
	player = config.child("player").child("blackMage");

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

	// Copying values of the speed
	pugi::xml_node speed = player.child("speed");

	horizontalSpeed = speed.child("movement").attribute("horizontal").as_float();
	godModeSpeed = speed.child("movement").attribute("godmode").as_float();
}