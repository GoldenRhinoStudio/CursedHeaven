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
#include "j1DragoonKnight.h"
#include "j1BlackMage.h"
#include "j1Scene1.h"
#include "j1Scene2.h"
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

void j1Shop::PlaceShopScene1()
{
	// Creating the shop	
	restartingShop = false;

	int item1 = (rand() % 5);
	App->shop->CreateItem(ITEM_TYPE(item1), -1100, 725);

	int item2 = (rand() % 5);
	if (item2 == item1) item2++;
	if (item2 > 4) item2 = 0;
	App->shop->CreateItem(ITEM_TYPE(item2), -1050, 765);

	int item3 = (rand() % 5);
	if (item3 == item1) item3--;
	if (item3 < 0) item3 = 4;
	if (item3 == item2) item3++;
	if (item3 > 4) item3 = 0;
	if (item3 == item1) item3++;
	if (item3 > 4) item3 = 0;
	App->shop->CreateItem(ITEM_TYPE(item3), -1000, 725);

	App->shop->CreateItem(POTION, -1050, 680);
	App->entity->CreateEntity(SELLER, -1055, 715);
}

void j1Shop::PlaceShopScene2()
{
	// Creating the shop	
	restartingShop = false;

	int item1 = (rand() % 5);
	App->shop->CreateItem(ITEM_TYPE(item1), -655, 515);

	int item2 = (rand() % 5);
	if (item2 == item1) item2++;
	if (item2 > 4) item2 = 0;
	App->shop->CreateItem(ITEM_TYPE(item2), -705, 555);

	int item3 = (rand() % 5);
	if (item3 == item1) item3--;
	if (item3 < 0) item3 = 4;
	if (item3 == item2) item3++;
	if (item3 > 4) item3 = 0;
	if (item3 == item1) item3++;
	if (item3 > 4) item3 = 0;
	App->shop->CreateItem(ITEM_TYPE(item3), -755, 515);

	App->shop->CreateItem(POTION, -705, 480);
	App->entity->CreateEntity(SELLER, -710, 505);
}

bool j1Shop::Update(float dt)
{
	BROFILER_CATEGORY("ShopUpdate", Profiler::Color::LightSeaGreen)
	
	return true;
}

bool j1Shop::Load(pugi::xml_node& data)
{
	potions = data.append_child("potions").attribute("number").as_uint();
	bootsLevel = data.append_child("boots").attribute("level").as_uint();
	swordLevel = data.append_child("sword").attribute("level").as_uint();
	heartLevel = data.append_child("heart").attribute("level").as_uint();
	armourLevel = data.append_child("armour").attribute("level").as_uint();
	hourglassLevel = data.append_child("hourglass").attribute("level").as_uint();
	bookLevel = data.append_child("book").attribute("level").as_uint();

	return true;
}

bool j1Shop::Save(pugi::xml_node& data) const
{
	pugi::xml_node poti = data.append_child("potions");
	pugi::xml_node boot = data.append_child("boots");
	pugi::xml_node sword = data.append_child("sword");
	pugi::xml_node heart = data.append_child("heart");
	pugi::xml_node arm = data.append_child("armour");
	pugi::xml_node hour = data.append_child("hourglass");
	pugi::xml_node book = data.append_child("book");

	poti.append_attribute("number") = potions;
	boot.append_attribute("level") = bootsLevel;
	sword.append_attribute("level") = swordLevel;
	heart.append_attribute("level") = heartLevel;
	arm.append_attribute("level") = armourLevel;
	hour.append_attribute("level") = hourglassLevel;
	book.append_attribute("level") = bookLevel;

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

	if (restartingShop) {
		potions = 0;
		bootsLevel = 0;
		swordLevel = 0;
		heartLevel = 0;
		armourLevel = 0;
		hourglassLevel = 0;
		bookLevel = 0;
	}

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
	height = 2;

	// Audios are loaded
	LOG("Loading player audios");

	if (type == POTION) collider = App->collisions->AddCollider({ (int)position.x, (int)position.y, 15, 25 }, COLLIDER_ITEM, App->entity);
	else collider = App->collisions->AddCollider({ (int)position.x, (int)position.y, 22, 22 }, COLLIDER_ITEM, App->entity);

	switch (type) {
	case BOOTS:
		description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Speed +1: 3C", App->gui->beige);
		image = { 0, 919, 160, 205 };
		prize = App->shop->boots_prize1;
		break;

	case SWORD:
		description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Attack +1: 3C", App->gui->beige);
		image = { 0, 708, 211, 211 };
		prize = App->shop->sword_prize1;
		break;

	case HEART:
		description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Life +1: 3C", App->gui->beige);
		image = { 170, 0, 217, 217 };
		prize = App->shop->heart_prize1;
		break;

	case HOURGLASS:
		description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "CDR +1: 3C", App->gui->beige);
		image = { 887, 0, 134, 206 };
		prize = App->shop->hourglass_prize1;
		break;

	case BOOK:
		description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Ability +1: 3C", App->gui->beige);
		image = { 0, 496, 162, 212 };
		prize = App->shop->book_prize1;
		break;

	case POTION:
		description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Potion: 3C", App->gui->beige);
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
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Speed +2: 8C", App->gui->beige);
		}
		else if (App->shop->bootsLevel == 2) {
			image = { 320, 919, 160, 205 };
			prize = App->shop->boots_prize3;
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Speed +3: 15C", App->gui->beige);
		}
		else if (App->shop->bootsLevel > 2)
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Max. level", App->gui->beige);
		break;

	case SWORD:
		if (App->shop->swordLevel == 1) {
			image = { 211, 708, 211, 211 };
			prize = App->shop->sword_prize2;
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Attack +2: 8C", App->gui->beige);
		}
		else if (App->shop->swordLevel == 2) {
			image = { 422, 708, 211, 211 };
			prize = App->shop->sword_prize3;
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Attack +3: 15C", App->gui->beige);
		}
		else if (App->shop->swordLevel > 2)
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Max. level", App->gui->beige);
		break;

	case HEART:
		if (App->shop->heartLevel == 1) {
			image = { 387, 0, 217, 217 };
			prize = App->shop->heart_prize2;
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Life +2: 8C", App->gui->beige);
		}
		else if (App->shop->heartLevel == 2) {
			image = { 604, 0, 217, 217 };
			prize = App->shop->heart_prize3;
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Life +3: 15C", App->gui->beige);
		}
		else if (App->shop->heartLevel > 2)
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Max. level", App->gui->beige);
		break;

	case HOURGLASS:
		if (App->shop->hourglassLevel == 1) {
			image = { 887, 207, 134, 206 };
			prize = App->shop->hourglass_prize2;
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "CDR +2: 8C", App->gui->beige);
		}
		else if (App->shop->hourglassLevel == 2) {
			image = { 887, 414, 134, 206 };
			prize = App->shop->hourglass_prize3;
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "CDR +3: 15C", App->gui->beige);
		}
		else if (App->shop->hourglassLevel > 2)
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Max. level", App->gui->beige);
		break;

	case BOOK:
		if (App->shop->bookLevel == 1) {
			image = { 163, 496, 162, 212 };
			prize = App->shop->book_prize2;
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Ability +2: 8C", App->gui->beige);
		}
		else if (App->shop->bookLevel == 2) {
			image = { 326, 496, 162, 212 };
			prize = App->shop->book_prize3;
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Ability +3: 15C", App->gui->beige);
		}
		else if (App->shop->bookLevel > 2)
			description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Max. level", App->gui->beige);
		break;

	case POTION:
		if (App->scene1->potionCounter == 3 || App->scene2->potionCounter == 3) collider->type = COLLIDER_NONE;
	}

	return true;
}

bool j1Item::DrawOrder(float dt)
{
	// Drawing the seller
	if (type == POTION && App->scene1->potionCounter < 3 && App->scene2->potionCounter < 3)
		Draw(&image, false, 0,0,0.09f);
	else if (type != POTION)
		Draw(&image, false, 0,0,0.1f);

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
					if (App->shop->bootsLevel == 0) aux = App->entity->currentPlayer->movementSpeed * 0.1f;
					else if (App->shop->bootsLevel == 1) aux = App->entity->currentPlayer->movementSpeed * 0.15f;
					else if (App->shop->bootsLevel == 2) aux = App->entity->currentPlayer->movementSpeed * 0.25f;

					App->entity->currentPlayer->movementSpeed += aux;
					App->shop->bootsLevel++;
					break;

				case SWORD:
					if (App->shop->swordLevel == 0) dmg = App->entity->currentPlayer->basicDamage * 0.1f;
					else if (App->shop->swordLevel == 1) dmg = App->entity->currentPlayer->basicDamage * 0.15f;
					else if (App->shop->swordLevel == 2) dmg = App->entity->currentPlayer->basicDamage * 0.25f;

					App->entity->currentPlayer->basicDamage += dmg;
					App->shop->swordLevel++;
					break;

				case HEART:
					if (App->shop->heartLevel == 0)	App->entity->currentPlayer->totalLifePoints += 30;
					else if (App->shop->heartLevel == 1) App->entity->currentPlayer->totalLifePoints += 55;
					else if (App->shop->heartLevel == 2) App->entity->currentPlayer->totalLifePoints += 80;

					App->shop->heartLevel++;
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
					if (App->shop->bookLevel == 0) {
						if (App->entity->player_type == MAGE) App->entity->mage->fireDamage += 100;
						else if (App->entity->player_type == KNIGHT) App->entity->knight->rageDamage += 80;
					}
					else if (App->shop->bookLevel == 1) {
						if (App->entity->player_type == MAGE) App->entity->mage->fireDamage += 150;
						else if (App->entity->player_type == KNIGHT) App->entity->knight->rageDamage += 130;
					}
					else if (App->shop->bookLevel == 2) {
						if (App->entity->player_type == MAGE) App->entity->mage->fireDamage += 250;
						else if (App->entity->player_type == KNIGHT) App->entity->knight->rageDamage += 200;
					}

					App->shop->bookLevel++;
					break;

				case POTION:
					if (App->shop->potions < 3 && App->scene1->potionCounter < 3) {
						App->shop->potions++;

						if (App->scene1->active) App->scene1->potionCounter++;
						else if (App->scene2->active) App->scene2->potionCounter++;

						if (App->shop->potions == 3)
							description = App->gui->CreateLabel(&App->shop->itemLabels, LABEL, (int)position.x, (int)position.y, App->gui->font1, "Max. potions", App->gui->beige);
					}
					break;
				}

			}
		}

		description->Draw(0.7, -8);
	}
}