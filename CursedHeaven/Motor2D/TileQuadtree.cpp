#include "p2Defs.h"
#include "p2Point.h"
#include "p2Log.h"
#include "SDL/include/SDL.h"
#include <assert.h>

#include "j1App.h"
#include "j1Map.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Textures.h"
#include "j1Render.h"




TileQuadtree::TileQuadtree(uint max_levels, SDL_Rect section, uint size, uint level) : Quadtree(max_levels, section, level), size(size)
{
	//Set the subnodes as nullptr
	for (int i = 0; i < 4; i++)
		nodes[i] = nullptr;

	/*We will only store the tiles in the bottom nodes,
	so, if this node will have subnodes, it won't need room for tiles*/
	if (level != max_levels)
		tiles = NULL;

	else
	{
		tiles = new TileData[size];

		for (int i = 0; i < size; ++i)
			tiles[i] = TileData();
	}

	tiles_contained = 0;

	assert(level <= max_levels, "MAX LEVELS > LEVELS");
}

void TileQuadtree::CleanUp()
{
	if (divided == true)
	{
		for (int i = 0; i < 4; i++)
		{
			nodes[i]->CleanUp();
			nodes[i] = nullptr;
		}

	}
	RELEASE(tiles);
}

void TileQuadtree::Split()
{
	if (level < max_levels && divided == false)
	{
		nodes[0] = new TileQuadtree(max_levels, { section.x,section.y, section.w / 2, section.h / 2 }, size / 4, level + 1);
		nodes[1] = new TileQuadtree(max_levels, { section.x + section.w / 2,section.y, section.w / 2,section.h / 2 }, size / 4, level + 1);
		nodes[2] = new TileQuadtree(max_levels, { section.x,section.y + section.h / 2 , section.w / 2, section.h / 2 }, size / 4, level + 1);
		nodes[3] = new TileQuadtree(max_levels, { section.x + section.w / 2,section.y + section.h / 2, section.w / 2,section.h / 2 }, size / 4, level + 1);
		divided = true;
	}

}

void TileQuadtree::InsertTile(TileData* tile)
{
	SDL_Rect tile_rect;
	tile_rect.x = tile->x;
	tile_rect.y = tile->y;
	tile_rect.w = App->map->data.tile_width;
	tile_rect.h = App->map->data.tile_height;

	//If the node is in the lowest level, store the tile here
	if (level == max_levels)
	{
		tiles[tiles_contained++] = *tile;
	}

	//In case there are lower subnodes, it will be stored there
	else if (level < max_levels)
	{
		if (this->CheckTouch({ tile_rect }))
		{
			//If there are no subnodes yet, split the current node
			if (divided == false)
				Split();

			for (int i = 3; i >= 0; --i)
			{
				if (nodes[i]->CheckTouch({ tile_rect }))
				{
					if (nodes[i]->tiles_contained < nodes[i]->size)
					{
						nodes[i]->InsertTile(tile);
						break;
					}
				}
			}
		}
	}
}

bool TileQuadtree::CheckVisibility()
{
	int scale = App->win->GetScale();
	int offset = 5 * scale;
	uint screen_w1;
	uint screen_h2;
	App->win->GetWindowSize(screen_w1, screen_h2);


	int camera_x = -App->render->camera.x / scale;
	int camera_y = -App->render->camera.y / scale;
	int screen_w = screen_w1 / scale;
	int screen_h = screen_h2 / scale;

	if (camera_x > (section.x + section.w + offset) ||
		(camera_x + int(screen_w)) < section.x - offset ||
		camera_y > (section.y + section.h + offset) ||
		(camera_y + int(screen_h)) < section.y - offset)
		return false;

	return true;
}

void TileQuadtree::DrawMap() 
{
	if (CheckVisibility())
	{
		if (level == max_levels)
		{
			for (int i = 0; i < tiles_contained; ++i)
			{
				TileData* img2 = &tiles[i];
				TileSet* tileset = App->map->GetTilesetFromTileId(img2->id);
				App->map->Rectvec.push_back(new SDL_Rect(tileset->GetTileRect(img2->id)));
				img2->texture = tileset->texture;
				img2->speed = 1.0f;
				img2->section = App->map->Rectvec.back();
				App->render->map_sprites_priority.push(img2);
			}
		}

		if (divided == true)
		{
			for (int i = 0; i < 4; ++i)
			{
				nodes[i]->DrawMap();
			}
		}
	}
}


void TileQuadtree::DrawQuadtree()
{
	if (CheckVisibility() == true)
	{
		if (level == 1)
		{
			App->render->DrawLine(section.x, section.y, section.x, section.y + section.h, 0, 0, 255);
			App->render->DrawLine(section.x, section.y, section.x + section.w, section.y, 0, 0, 255);
			App->render->DrawLine(section.x, section.y + section.h, section.x + section.w, section.y + section.h, 0, 0, 255);
			App->render->DrawLine(section.x + section.w, section.y, section.x + section.w, section.y + section.h, 0, 0, 255);
		}

		if (divided == true)
		{
			App->render->DrawLine(section.x + section.w / 2, section.y, section.x + section.w / 2, section.y + section.h, 0, 0, 255);
			App->render->DrawLine(section.x, section.y + section.h / 2, section.x + section.w, section.y + section.h / 2, 0, 0, 255);

			if (level < max_levels - 1)
			{
				for (int i = 0; i < 4; ++i)
				{
					nodes[i]->DrawQuadtree();
				}
			}
		}
	}
}