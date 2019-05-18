#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1EntityManager.h"
#include "j1Player.h"
#include "j1Label.h"
#include "j1Shop.h"
#include "j1Seller.h"
#include "j1Scene1.h"
#include "j1Input.h"

#include "Brofiler/Brofiler.h"
#include <time.h>

j1Shop::j1Shop()
{
	name.assign("shop");
	srand(time(NULL));
}

j1Shop::~j1Shop() {}

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

	potion_prize = p.child("potion").attribute("p").as_uint();
	potionHealing = p.child("potion").attribute("healing").as_uint();

	return true;
}

bool j1Shop::Start() {

	return true;
}

void j1Shop::PlaceShop()
{
	// Creating the shop
	int item1 = (rand() % 6);
	App->shop->CreateItem(POTION, -1100, 715);

	int item2 = (rand() % 6);
	App->shop->CreateItem(HEART, -1050, 745);

	int item3 = (rand() % 6);
	App->shop->CreateItem(HEART, -1000, 715);

	App->entity->CreateEntity(SELLER, -1050, 705);
}

bool j1Shop::Update(float dt)
{
	BROFILER_CATEGORY("ShopUpdate", Profiler::Color::LightSeaGreen)


		return true;
}

bool j1Shop::CleanUp()
{
	LOG("Freeing all items");

	for (std::list<j1Label*>::iterator item = itemLabels.begin(); item != itemLabels.end(); ++item) {
		(*item)->CleanUp();
		itemLabels.remove(*item);
	}

	for (std::list<j1Item*>::iterator item = items.begin(); item != items.end(); ++item) {
		(*item)->CleanUp();
		items.remove(*item);
	}

	potions = 0;
	bootsLevel = 0;
	swordLevel = 0;
	heartLevel = 0;
	armourLevel = 0;
	hourglassLevel = 0;
	bookLevel = 0;

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

j1Item::~j1Item() {}

bool j1Item::Start() {
	// Textures are loaded
	LOG("Loading player textures");
	sprites = App->tex->Load("textures/Collectibles/items.png");

	// Audios are loaded
	LOG("Loading player audios");

	if (type == POTION) collider = App->collisions->AddCollider({ (int)position.x, (int)position.y, 15, 25 }, COLLIDER_ITEM, App->entity);
	else collider = App->collisions->AddCollider({ (int)position.x, (int)position.y, 22, 22 }, COLLIDER_ITEM, App->entity);

	switch (type) {
	case BOOTS:
		description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Speed +1: 30G", App->gui->beige);
		image = { 0, 919, 160, 205 };
		prize = App->shop->boots_prize1;
		break;

	case SWORD:
		description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Attack +1: 30G", App->gui->beige);
		image = { 0, 708, 211, 211 };
		prize = App->shop->sword_prize1;
		break;

	case HEART:
		description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Life +1: 30G", App->gui->beige);
		image = { 170, 0, 217, 217 };
		prize = App->shop->heart_prize1;
		break;

	case ARMOUR:
		description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Armour +1: 30G", App->gui->beige);
		break;

	case HOURGLASS:
		description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "CDR +1: 30G", App->gui->beige);
		image = { 887, 0, 134, 206 };
		prize = App->shop->hourglass_prize1;
		break;

	case BOOK:
		description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Ability +1: 30G", App->gui->beige);
		image = { 0, 496, 162, 212 };
		prize = App->shop->book_prize1;
		break;

	case POTION:
		description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Potion: 30G", App->gui->beige);
		image = { 0, 0, 170, 270 };
		prize = App->shop->potion_prize;
		break;
	}

	return true;
}

bool j1Item::Update(float dt, bool do_logic) {

	switch (type) {
	case BOOTS:
		if (App->shop->bootsLevel == 1) {
			image = { 160, 919, 160, 205 };
			prize = App->shop->boots_prize2;
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Speed +2: 80G", App->gui->beige);
		}
		else if (App->shop->bootsLevel == 2) {
			image = { 320, 919, 160, 205 };
			prize = App->shop->boots_prize3;
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Speed +3: 150G", App->gui->beige);
		}
		else if (App->shop->bootsLevel > 2)
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Max. level", App->gui->beige);
		break;

	case SWORD:
		if (App->shop->swordLevel == 1) {
			image = { 211, 708, 211, 211 };
			prize = App->shop->sword_prize2;
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Attack +2: 80G", App->gui->beige);
		}
		else if (App->shop->swordLevel == 2) {
			image = { 422, 708, 211, 211 };
			prize = App->shop->sword_prize3;
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Attack +3: 150G", App->gui->beige);
		}
		else if (App->shop->swordLevel > 2)
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Max. level", App->gui->beige);
		break;

	case HEART:
		if (App->shop->heartLevel == 1) {
			image = { 387, 0, 217, 217 };
			prize = App->shop->heart_prize2;
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Life +2: 80G", App->gui->beige);
		}
		else if (App->shop->heartLevel == 2) {
			image = { 604, 0, 217, 217 };
			prize = App->shop->heart_prize3;
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Life +3: 150G", App->gui->beige);
		}
		else if (App->shop->heartLevel > 2)
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Max. level", App->gui->beige);
		break;

	case ARMOUR:
		break;

	case HOURGLASS:
		if (App->shop->hourglassLevel == 1) {
			image = { 887, 207, 134, 206 };
			prize = App->shop->hourglass_prize2;
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "CDR +2: 80G", App->gui->beige);
		}
		else if (App->shop->hourglassLevel == 2) {
			image = { 887, 414, 134, 206 };
			prize = App->shop->hourglass_prize3;
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "CDR +3: 150G", App->gui->beige);
		}
		else if (App->shop->hourglassLevel > 2)
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Max. level", App->gui->beige);
		break;

	case BOOK:
		if (App->shop->bookLevel == 1) {
			image = { 163, 496, 162, 212 };
			prize = App->shop->book_prize2;
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Ability +2: 80G", App->gui->beige);
		}
		else if (App->shop->bookLevel == 2) {
			image = { 326, 496, 162, 212 };
			prize = App->shop->book_prize3;
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Ability +3: 150G", App->gui->beige);
		}
		else if (App->shop->bookLevel > 2)
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Max. level", App->gui->beige);
		break;

	case POTION:
		if (App->scene1->potionCounter == 3) collider->type = COLLIDER_NONE;
	}

	// Blitting the item
	if (type == POTION && App->scene1->potionCounter < 3) App->render->Blit(sprites, position.x, position.y, &image, SDL_FLIP_NONE, 1.0f, 0.09f);
	else if (type != POTION) App->render->Blit(sprites, position.x, position.y, &image, SDL_FLIP_NONE, 1.0f, 0.1f);

	return true;
}

bool j1Item::CleanUp() {

	// Remove all memory leaks
	LOG("Unloading the player");
	App->tex->UnLoad(sprites);

	if (collider != nullptr)
		collider->to_delete = true;

	return true;
}

void j1Item::OnCollision(Collider* c1, Collider* c2) {
	if (c2->type == COLLIDER_PLAYER) {

		if (level > 2 && type != POTION)
			App->render->DrawQuad({ (int)position.x - 15, (int)position.y, 50, 10 }, 0, 96, 255, 160);
		else if (App->entity->currentPlayer->coins < prize || (App->shop->potions == 3 && type == POTION)) {

			App->render->DrawQuad({ (int)position.x - 15, (int)position.y, 50, 10 }, 255, 0, 0, 160);
		}
		else {
			App->render->DrawQuad({ (int)position.x - 15, (int)position.y, 50, 10 }, 0, 0, 0, 160);

			if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN
				|| (SDL_GameControllerGetButton(App->input->controller, SDL_CONTROLLER_BUTTON_A) == KEY_DOWN
					&& (App->shop->buyingTime.Read() >= App->shop->lastBuyingTime + 500))) {

				App->shop->lastBuyingTime = App->shop->buyingTime.Read();
				App->entity->currentPlayer->coins -= prize;
				level++;

				uint aux = 0;
				uint dmg = 0;
				uint cdrE = 0;
				uint cdrQ = 0;

				switch (type) {
				case BOOTS:
					if (App->shop->bootsLevel == 0) aux = (uint)(App->entity->currentPlayer->speed * 0.1f);
					else if (App->shop->bootsLevel == 1) aux = (uint)(App->entity->currentPlayer->speed * 0.15f);
					else if (App->shop->bootsLevel == 2) aux = (uint)(App->entity->currentPlayer->speed * 0.25f);

					App->entity->currentPlayer->speed += aux;
					App->shop->bootsLevel++;
					break;

				case SWORD:
					if (App->shop->swordLevel == 0) aux = (uint)(App->entity->currentPlayer->basicDamage * 0.1f);
					else if (App->shop->swordLevel == 1) aux = (uint)(App->entity->currentPlayer->basicDamage * 0.15f);
					else if (App->shop->swordLevel == 2) aux = (uint)(App->entity->currentPlayer->basicDamage * 0.25f);

					App->entity->currentPlayer->basicDamage += aux;
					App->shop->swordLevel++;
					break;

				case HEART:
					if (App->shop->heartLevel == 0)	App->entity->currentPlayer->totalLifePoints += 30;
					else if (App->shop->heartLevel == 1) App->entity->currentPlayer->totalLifePoints += 55;
					else if (App->shop->heartLevel == 2) App->entity->currentPlayer->totalLifePoints += 80;

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
					if (App->shop->potions < 3 && App->scene1->potionCounter < 3) {
						App->shop->potions++;
						App->scene1->potionCounter++;
					}
					break;
				}

			}
		}

		description->Draw(0.7, -8);
	}
}