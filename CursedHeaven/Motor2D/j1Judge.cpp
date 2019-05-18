#include "j1App.h"
#include "j1Judge.h"
#include "j1Entity.h"
#include "j1EntityManager.h"
#include "j1Scene1.h"
#include "j1Textures.h"
#include "j1Render.h"
#include <time.h>

j1Judge::j1Judge(int x, int y, ENTITY_TYPES type) : j1Entity(x, y, ENTITY_TYPES::JUDGE) 
{
	srand(time(NULL));
}


j1Judge::~j1Judge()
{
}

bool j1Judge::Start()
{
	Jtex = App->tex->Load("textures/character/judge/Judge.png");
	return true;
}

bool j1Judge::Update(float dt, bool do_logic)
{
	SDL_Rect judgeRect = { 42, 2, 22, 33 };

	Draw(&judgeRect, true, 225, 750, 1.0f);

	return true;
}

