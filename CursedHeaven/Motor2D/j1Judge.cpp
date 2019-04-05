#include "j1App.h"
#include "j1Judge.h"
#include "j1Entity.h"
#include "j1EntityManager.h"
#include "j1Scene1.h"
#include <time.h>

j1Judge::j1Judge(int x, int y, ENTITY_TYPES type) : j1Entity(x, y, ENTITY_TYPES::NPC) 
{
	srand(time(NULL));
}


j1Judge::~j1Judge()
{
}

bool j1Judge::Start()
{
	RandomLaw();
	return true;
}

LawType j1Judge::RandomLaw()
{
	LawType chosenLaw;

	int law = (rand() % 4) + 1;

	if (law == 1) {
		chosenLaw = Q_ABILITY;
		LOG("q");
	}

	else if (law == 2) {
		chosenLaw = E_ABILITY;
		LOG("e");
	}
		
	else if (law == 3) {
		chosenLaw = EVERY_ROOM;
		LOG("room");
	}
		
	else if (law == 4) {
		chosenLaw = HEALTH;
		LOG("health");
	}
		
	return chosenLaw;
}
