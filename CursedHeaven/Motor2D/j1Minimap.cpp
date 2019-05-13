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

bool j1Minimap::Awake(pugi::xml_node & config)
{
	return true;
}

bool j1Minimap::Start()
{	

	return true;
}


bool j1Minimap::Update(float dt)
{	
	return true;
}

bool j1Minimap::PostUpdate()
{	
	DrawEntities();
	return true;
}

bool j1Minimap::CleanUp()
{
	return true;
}

bool j1Minimap::MinimapCoords(int& map_x, int& map_y)
{
//	int mouse_x, mouse_y;
//	App->input->GetMousePosition(mouse_x, mouse_y);
//
//	if (mouse_x >= 0 && mouse_x <= minimap_width	&&	mouse_y >= 0 && mouse_y <= minimap_height)
//	{
//		// TODO 9: Assign to "map_x" and "map_y" the mouse position respect the minimap, to the
//		// position that corresponds to the map
//		// Take into account that it is an isometric map
//		map_x = (mouse_x - minimap_width / 2) / minimap_scale;
//		map_y = mouse_y / minimap_scale;
//	}
//
//	else
//		return false;

	return true;
}


//void j1Minimap::DrawCamera()
//{
//	// TODO 8: Fill the parameters of "map_camera"	to see in the minimap which part of the map are you seeing
//	// Take into account that it is an isometric map
//	// Reminder: The camera values are negative
//	SDL_Rect map_camera = { ((-App->render->camera.x)) * minimap_scale + minimap_width / 2, -App->render->camera.y * minimap_scale, 
//							App->render->camera.w * minimap_scale, App->render->camera.h * minimap_scale };
//
//	App->render->DrawQuad(map_camera, 255, 255, 0, 255, false, false);
//}
//
//void j1Minimap::MinimapBorders()
//{
//	App->render->DrawLine(x_offset, minimap_height / 2 + y_offset, minimap_width / 2 + x_offset, y_offset, 255, 255, 255, 255, false);
//	App->render->DrawLine(minimap_width + x_offset, minimap_height/ 2 + y_offset, minimap_width / 2 + x_offset, y_offset, 255, 255, 255, 255, false);
//	App->render->DrawLine(minimap_width + x_offset, minimap_height / 2 + y_offset, minimap_width / 2 + x_offset, minimap_height + y_offset, 255, 255, 255, 255, false);
//	App->render->DrawLine(x_offset, minimap_height / 2 + y_offset, minimap_width / 2 + x_offset, minimap_height + y_offset, 255, 255, 255, 255, false);
//}

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


