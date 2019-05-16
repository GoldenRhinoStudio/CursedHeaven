#include "j1Minimap.h"

#include "j1App.h"
#include "j1Window.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Input.h"

#include "j1Map.h"
#include "j1Entity.h"
#include "j1EntityManager.h"

#include "p2Log.h"

#include <list>

j1Minimap::j1Minimap()
{
	name = "minimap";
}

j1Minimap::~j1Minimap() {}

bool j1Minimap::Update(float dt)
{	
	return true;
}

bool j1Minimap::PostUpdate()
{	
	DrawEntities();
	return true;
}

void j1Minimap::DrawEntities()
{
	int pos_x, pos_y;

	for (std::list<j1Entity*>::iterator item = App->entity->entities.begin(); item != App->entity->entities.end(); ++item) {
		pos_x = (*item)->position.x * 0.125f;
		pos_y = (*item)->position.y * 0.125f;

		if ((*item)->type == ENTITY_TYPES::PLAYER)
			App->render->DrawQuad({ pos_x + (int)App->win->width / 2 + 310, pos_y + (int)App->win->height / 2 + 185, 4, 4 }, 255, 0, 0, 255, true, false);
		
		else if ((*item)->type == ENTITY_TYPES::SLIME)
			App->render->DrawQuad({ pos_x + (int)App->win->width / 2 + 310, pos_y + (int)App->win->height / 2 + 185, 4, 4 }, 0, 0, 255, 255, true, false);
	}
}