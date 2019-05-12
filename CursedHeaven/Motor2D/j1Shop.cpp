#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1EntityManager.h"
#include "j1Player.h"
#include "j1Shop.h"
#include "j1Map.h"

#include "Brofiler/Brofiler.h"

j1Shop::j1Shop()
{
	name.assign("shop");
}

j1Shop::~j1Shop() {}

bool j1Shop::Start()
{

	return true;
}

bool j1Shop::PreUpdate()
{
	BROFILER_CATEGORY("ShopPreUpdate", Profiler::Color::Orange)

	

	return true;
}

bool j1Shop::Awake(pugi::xml_node& config)
{
	LOG("Awaking shop");
	pugi::xml_node p = config.child("prizes");

	// Prizes
	boots_prize1 = p.child("boots").attribute("p1").as_uint();
	boots_prize2 = p.child("boots").attribute("p2").as_uint();
	boots_prize3 = p.child("boots").attribute("p3").as_uint();

	sword_prize1 = p.child("sword").attribute("p1").as_uint();
	sword_prize2 = p.child("sword").attribute("p2").as_uint();
	sword_prize3 = p.child("sword").attribute("p3").as_uint();

	heart_prize1 = p.child("heart").attribute("p1").as_uint();
	heart_prize2 = p.child("heart").attribute("p2").as_uint();
	heart_prize3 = p.child("heart").attribute("p3").as_uint();

	hourglass_prize1 = p.child("hourglass").attribute("p1").as_uint();
	hourglass_prize2 = p.child("hourglass").attribute("p2").as_uint();
	hourglass_prize3 = p.child("hourglass").attribute("p3").as_uint();

	armour_prize1 = p.child("armour").attribute("p1").as_uint();
	armour_prize2 = p.child("armour").attribute("p2").as_uint();
	armour_prize3 = p.child("armour").attribute("p3").as_uint();

	book_prize1 = p.child("book").attribute("p1").as_uint();
	book_prize2 = p.child("book").attribute("p2").as_uint();
	book_prize3 = p.child("book").attribute("p3").as_uint();

	potion_prize = p.child("sword").attribute("p").as_uint();

	return true;
}

bool j1Shop::Update(float dt)
{
	BROFILER_CATEGORY("ShopUpdate", Profiler::Color::LightSeaGreen)
	

	return true;
}

bool j1Shop::PostUpdate()
{
	BROFILER_CATEGORY("ShopPostUpdate", Profiler::Color::Yellow)

	return true;
}

bool j1Shop::CleanUp()
{
	LOG("Freeing all enemies");

	return true;
}

j1Item* j1Shop::CreateItem(ITEM_TYPE type, int x, int y)
{
	j1Item* ret = nullptr;

	ret = new j1Item(x, y, type);
	items.push_back(ret);

	App->entity->entities.push_back((j1Entity*)ret);

	return ret;
}

void j1Shop::OnCollision(Collider* c1, Collider* c2)
{
	for (std::list<j1Item*>::iterator item = items.begin(); item != items.end(); ++item)
	{
		if ((*item)->collider == c1)
		{
			(*item)->OnCollision(c1, c2);
			break;
		}
	}
}

// ------------------------------------------------

j1Item::j1Item(int x, int y, ITEM_TYPE type) 
	: j1Entity(x, y, ITEM), type(type) {}

j1Item::~j1Item(){}

bool j1Item::Start() {
	// Textures are loaded
	LOG("Loading player textures");
	sprites = App->tex->Load("textures/Collectibles/items.png");

	// Audios are loaded
	LOG("Loading player audios");

	collider = App->collisions->AddCollider({ (int)position.x, (int)position.y, 20, 20 }, COLLIDER_ITEM, App->entity);
	itemRefresh.Start();

	return true;
}

bool j1Item::Update(float dt, bool do_logic) {
	
	switch (type) {
	case BOOTS:
		if (App->shop->bootsLevel == 0) {
			image = {0, 0, 0, 0};
			prize = App->shop->boots_prize1;
		}
		else if (App->shop->bootsLevel == 1) {
			image = { 0, 0, 0, 0 };
			prize = App->shop->boots_prize2;
		}
		else if (App->shop->bootsLevel == 2) {
			image = { 0, 0, 0, 0 };
			prize = App->shop->boots_prize3;
		}
		break;

	case SWORD:
		if (App->shop->swordLevel == 0) {
			image = { 0, 0, 0, 0 };
			prize = App->shop->sword_prize1;
		}
		else if (App->shop->swordLevel == 1) {
			image = { 0, 0, 0, 0 };
			prize = App->shop->sword_prize2;
		}
		else if (App->shop->swordLevel == 2) {
			image = { 0, 0, 0, 0 };
			prize = App->shop->sword_prize3;
		}
		break;

	case HEART:
		if (App->shop->heartLevel == 0) {
			image = { 170, 0, 217, 217 };
			prize = App->shop->heart_prize1;
		}
		else if (App->shop->heartLevel == 1) {
			image = { 387, 0, 217, 217 };
			prize = App->shop->heart_prize2;
		}
		else if (App->shop->heartLevel == 2) {
			image = { 604, 0, 217, 217 };
			prize = App->shop->heart_prize3;
		}
		break;

	case ARMOUR:
		if (App->shop->armourLevel == 0) {
			image = { 0, 0, 0, 0 };
			prize = App->shop->armour_prize1;
		}
		else if (App->shop->armourLevel == 1) {
			image = { 0, 0, 0, 0 };
			prize = App->shop->armour_prize2;
		}
		else if (App->shop->armourLevel == 2) {
			image = { 0, 0, 0, 0 };
			prize = App->shop->armour_prize3;
		}
		break;

	case HOURGLASS:
		if (App->shop->hourglassLevel == 0) {
			image = { 0, 0, 0, 0 };
			prize = App->shop->hourglass_prize1;
		}
		else if (App->shop->hourglassLevel == 1) {
			image = { 0, 0, 0, 0 };
			prize = App->shop->hourglass_prize2;
		}
		else if (App->shop->hourglassLevel == 2) {
			image = { 0, 0, 0, 0 };
			prize = App->shop->hourglass_prize3;
		}
		break;

	case BOOK:
		if (App->shop->bookLevel == 0) {
			image = { 0, 0, 0, 0 };
			prize = App->shop->book_prize1;
		}
		else if (App->shop->bookLevel == 1) {
			image = { 0, 0, 0, 0 };
			prize = App->shop->book_prize2;
		}
		else if (App->shop->bookLevel == 2) {
			image = { 0, 0, 0, 0 };
			prize = App->shop->book_prize3;
		}
		break;

	case POTION:
		image = { 0, 0, 170, 270 };
		prize = App->shop->potion_prize;
	}

	// Blitting the item
	App->render->Blit(sprites, position.x, position.y, &image, SDL_FLIP_NONE, 1.0f, 0.1f);

	return true;
}

bool j1Item::DrawOrder(float dt) {


	return true;
}

bool j1Item::CleanUp() {

	// Remove all memory leaks
	LOG("Unloading the player");
	App->tex->UnLoad(sprites);

	return true;
}

void j1Item::OnCollision(Collider* c1, Collider* c2) {	
	if (c2->type == COLLIDER_PLAYER) {

		if (itemRefresh.Read() >= lastTime_refresh + 2000){
			canBeBought = true;
			lastTime_refresh = itemRefresh.Read();
		}

		if (canBeBought) {
			if (App->entity->currentPlayer->coins >= prize) {
				App->entity->currentPlayer->coins -= prize;

				uint aux = 0;
				uint cdrE = 0;
				uint cdrQ = 0;

				switch (type) {
				case BOOTS:
					if (App->shop->bootsLevel == 0) aux = App->entity->currentPlayer->speed * 0.1f;
					else if (App->shop->bootsLevel == 1) aux = App->entity->currentPlayer->speed * 0.15f;
					else if (App->shop->bootsLevel == 2) aux = App->entity->currentPlayer->speed * 0.25f;

					App->entity->currentPlayer->speed += aux;
					App->shop->bootsLevel++;
					break;

				case SWORD:
					if (App->shop->swordLevel == 0) App->entity->currentPlayer->basicDamage * 0.1f;
					else if (App->shop->swordLevel == 1) App->entity->currentPlayer->basicDamage * 0.15f;
					else if (App->shop->swordLevel == 2) App->entity->currentPlayer->basicDamage * 0.25f;

					App->shop->swordLevel++;
					break;

				case HEART:
					if (App->shop->heartLevel == 0){
						App->entity->currentPlayer->totalLifePoints + 30;
						App->entity->currentPlayer->lifePoints + 30;
					}
					else if (App->shop->heartLevel == 1) {
						App->entity->currentPlayer->totalLifePoints + 55;
						App->entity->currentPlayer->lifePoints + 55;
					}
					else if (App->shop->heartLevel == 2){
						App->entity->currentPlayer->totalLifePoints + 80;
						App->entity->currentPlayer->lifePoints + 80;
					}

					App->shop->heartLevel++;
					break;						

				case ARMOUR:
					//App->entity->currentPlayer->totalLifePoints + 25;
					break;

				case HOURGLASS:
					if (App->shop->hourglassLevel == 0) {
						cdrE = App->entity->currentPlayer->cooldownTime_E * 0.1f;
						cdrQ = App->entity->currentPlayer->cooldownTime_Q * 0.1f;
					}
					else if (App->shop->hourglassLevel == 1) {
						cdrE = App->entity->currentPlayer->cooldownTime_E * 0.15f;
						cdrQ = App->entity->currentPlayer->cooldownTime_Q * 0.15f;
					}
					else if (App->shop->hourglassLevel == 2) {
						cdrE = App->entity->currentPlayer->cooldownTime_E * 0.25f;
						cdrQ = App->entity->currentPlayer->cooldownTime_Q * 0.25f;
					}

					App->entity->currentPlayer->cooldownTime_E -= cdrE;
					App->entity->currentPlayer->cooldownTime_Q -= cdrQ;
					App->shop->hourglassLevel++;
					break;

				case BOOK:
					//App->entity->currentPlayer->totalLifePoints + 25;
					break;

				case POTION:
					if (App->shop->potions < 3)	App->shop->potions++;
					break;
				}

				canBeBought = false;
			}
		}
	}
}