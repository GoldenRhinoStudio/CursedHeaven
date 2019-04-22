#include "p2Defs.h"
#include "p2Log.h"

#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1DragoonKnight.h"
#include "j1BlackMage.h"
#include "j1Tank.h"
#include "j1Rogue.h"
#include "j1Collisions.h"
#include <math.h>

#include "Brofiler/Brofiler.h"

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.assign("map");
	draw_with_quadtrees = false;
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;
	draw_with_quadtrees = false;

	folder.assign(config.child("folder").child_value());

	bgBlitSpeed = config.child("blitSpeed").attribute("backgrounds").as_float();
	fogBlitSpeed = config.child("blitSpeed").attribute("fog").as_float();
	mapBlitSpeed = config.child("blitSpeed").attribute("map").as_float();

	return ret;
}

bool j1Map::PostUpdate() {
	return true;
}

void j1Map::Draw()
{
	BROFILER_CATEGORY("MapDraw", Profiler::Color::Red)

	if (map_loaded == false)
		return;

	tiles_rendered = 0;

	for (std::list<MapLayer*>::iterator layer = data.layers.begin(); layer != data.layers.end(); ++layer)
	{

		if ((*layer)->properties.Get("Nodraw") != 0) //No blit
			continue;

		if (draw_with_quadtrees) {
			(*layer)->tile_tree->DrawMap();
			//(*layer)->tile_tree->DrawQuadtree();
		}
		else {
			for (int y = 0; y < data.height; ++y)
			{
				for (int x = 0; x < data.width; ++x)
				{
					int tile_id = (*layer)->Get(x, y);
					if (tile_id > 0)
					{
						TileSet* tileset = GetTilesetFromTileId(tile_id);

						SDL_Rect r = tileset->GetTileRect(tile_id);
						iPoint pos = MapToWorld(x, y);

						if ((*layer)->name != "Meta0" && (*layer)->name != "Meta1" && (*layer)->name != "Meta2")
							App->render->Blit(tileset->texture, pos.x, pos.y, &r);
					}
				}
			}
		}
	}

	if (draw_with_quadtrees) {
		while (!App->render->map_sprites_priority.empty()) {
			App->render->map_sprites.push_back(App->render->map_sprites_priority.top());
			App->render->map_sprites_priority.pop();
		}
	}


}


int Properties::Get(const char* value, int default_value) const
{
	for (std::list<Property*>::const_iterator item = properties.begin(); item != properties.end(); ++item)
	{
		if ((*item)->name == value)
			return (*item)->value;
	}

	return default_value;
}

TileSet* j1Map::GetTilesetFromTileId(int id) const
{
	std::list<TileSet*>::const_iterator item = data.tilesets.begin();
	TileSet* set = *item;

	for ( item; item != data.tilesets.end(); ++item)
	{
		if (id < (*item)->firstgid)
		{
			set = *prev(item, 1);
			break;
		}

		set = *item;
	}

	return set;
}

iPoint j1Map::MapToWorld(int x, int y) const
{
	iPoint ret(0, 0);

	// Translates x,y coordinates from map positions to world positions
	if(data.type == MapTypes::MAPTYPE_ORTHOGONAL) {
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}
	else if (data.type == MapTypes::MAPTYPE_ISOMETRIC) {
		ret.x = (x - y) * (data.tile_width / 2);
		ret.y = (x + y) * (data.tile_height / 2);
	}

	return ret;
}

iPoint j1Map::WorldToMap(int x, int y) const
{
	iPoint ret(0,0);
	// Orthographic world to map coordinates
	if (data.type == MapTypes::MAPTYPE_ORTHOGONAL) {
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	// Isometric world to map coordinates
	else if (data.type == MapTypes::MAPTYPE_ISOMETRIC){
		ret.x = (x / (data.tile_width / 2) + y / (data.tile_height / 2)) / 2;
		ret.y = (y / (data.tile_height / 2) - x / (data.tile_width / 2)) / 2;
	}
	return ret;
}

// Method that receives a tile id and returns its rectangle
SDL_Rect TileSet::GetTileRect(int id) const
{
	SDL_Rect rect;
	int relative_id = id - firstgid;

	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));

	return rect;
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	for (std::list<TileSet*>::iterator item = data.tilesets.begin(); item != data.tilesets.end(); ++item)
	{
		delete (*item);
	}
	data.tilesets.clear();

	// Remove all layers
	for (std::list<MapLayer*>::iterator item = data.layers.begin(); item != data.layers.end(); ++item)
	{
		delete (*item);
	}
	data.layers.clear();

	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	std::string tmp = folder.data();
	tmp += file_name;

	pugi::xml_parse_result result = map_file.load_file(tmp.data());

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if(ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for(tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.push_back(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	height = 0;
	order = 0;
	for(layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);
		height++;

		if(ret == true)
			data.layers.push_back(lay);
	}

	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		for (std::list<TileSet*>::iterator item = data.tilesets.begin(); item != data.tilesets.end(); ++item)
		{
			TileSet* s = (*item);
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.data(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
		}

		for (std::list<MapLayer*>::iterator item = data.layers.begin(); item != data.layers.end(); ++item)
		{
			MapLayer* l = (*item);
			LOG("Layer ----");
			LOG("name: %s", l->name.data());
			LOG("tile width: %d tile height: %d", l->width, l->height);
		}
	}

	PutColliders(file_name);

	map_loaded = ret;

	return ret;
}

// Load map general properties
bool j1Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		std::string bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if(bg_color.size() > 0)
		{
			std::string red, green, blue;
			red = bg_color.substr(1, 2);
			green = bg_color.substr(3, 4);
			blue = bg_color.substr(5, 6);

			int v = 0;

			sscanf_s(red.data(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.data(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.data(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.b = v;
		}

		std::string orientation(map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.assign(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if(offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.data(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if(set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if(set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

bool j1Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_int();
	layer->height = node.attribute("height").as_int();
	LoadProperties(node, layer->properties);
	pugi::xml_node layer_data = node.child("data");

	if (draw_with_quadtrees) {
		//TEST
		iPoint layer_size;
		iPoint quadT_position(0, 0);
		switch (data.type)
		{
		case MAPTYPE_ORTHOGONAL:
			layer_size.x = layer->width*App->map->data.tile_width;
			layer_size.y = layer->height*App->map->data.tile_height;
			quadT_position.x = 0;
			break;
		case MAPTYPE_ISOMETRIC:
			layer_size.x = (layer->width + layer->height)*(App->map->data.tile_width *0.5f);
			layer_size.y = (layer->width + layer->height + 1) * (data.tile_height *0.5f);
			quadT_position.x = -layer_size.x + ((layer->width + 1)*App->map->data.tile_width / 2);
			break;
		}
		layer->tile_tree = new TileQuadtree(6, { quadT_position.x, 0, layer_size.x,layer_size.y }, layer->width*layer->height * 3);
	}
	//TEST

	if (layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{

		layer->data = new uint[layer->width*layer->height];
		memset(layer->data, 0, layer->width*layer->height);

		if (!draw_with_quadtrees) {
			int i = 0;
			for (pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
			{
				layer->data[i++] = tile.attribute("gid").as_int(0);
			}
		}
		else {
			int i = 0;
			for (pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
			{
				//TEST
				int id = tile.attribute("gid").as_int(0);
				if (id != 0) {
					//TEST
					iPoint tile_map_coordinates(App->map->MapToWorld((i - int(i / layer->width)*layer->width), int(i / layer->width)));
					float tile_height = (id % 2 == 0) ? 0.0f : 1.0f;
					TileData* tiledd = new TileData(id, tile_map_coordinates.x, tile_map_coordinates.y, order++, height + (tile_height + height));
					layer->tile_tree->InsertTile(tiledd);
					//TEST
				}
				layer->data[i++] = id;
			}
		}
	}

	return ret;
}

// Load a group of properties from a node and fill a list with it
bool j1Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = false;

	pugi::xml_node data = node.child("properties");

	if (data != NULL)
	{
		pugi::xml_node prop;

		for (prop = data.child("property"); prop; prop = prop.next_sibling("property"))
		{
			Properties::Property* p = new Properties::Property();

			p->name = prop.attribute("name").as_string();
			p->value = prop.attribute("value").as_int();

			properties.properties.push_back(p);
		}
	}

	return ret;
}

bool j1Map::PutColliders(const char * file_name)
{
	bool ret = true;

	std::string tmp = folder.data();
	tmp += file_name;

	pugi::xml_parse_result result = map_file.load_file(tmp.data());

	if (result == NULL)
	{
		LOG("Could not load tiled xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}
	pugi::xml_node obj;
	pugi::xml_node group;
	const char* object_name;
	for (group = map_file.child("map").child("objectgroup"); group && ret; group = group.next_sibling("objectgroup"))
	{
		object_name = group.attribute("name").as_string();

		for (obj = group.child("object"); obj && ret; obj = obj.next_sibling("object"))
		{
			/*if (strcmp(object_name, "map_collisions") == 0)
				App->collisions->AddCollider({ obj.attribute("x").as_int(), obj.attribute("y").as_int(), obj.attribute("width").as_int(), obj.attribute("height").as_int() }, COLLIDER_WALL);
			if (strcmp(object_name, "death_collisions") == 0)
				App->collisions->AddCollider({ obj.attribute("x").as_int(), obj.attribute("y").as_int(), obj.attribute("width").as_int(), obj.attribute("height").as_int() }, COLLIDER_DEATH);
			if (strcmp(object_name, "win_collider") == 0)
				App->collisions->AddCollider({ obj.attribute("x").as_int(), obj.attribute("y").as_int(), obj.attribute("width").as_int(), obj.attribute("height").as_int() }, COLLIDER_WIN)*/;
		}
	}

	return ret;
}

bool j1Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
	bool ret = false;

	for (std::list<MapLayer*>::const_iterator item = data.layers.begin(); item != data.layers.end(); ++item)
	{
		MapLayer* layer = (*item);

		if (layer->properties.Get("Navigation", 0) == 0)
			continue;

		uchar* map = new uchar[layer->width*layer->height];
		memset(map, 1, layer->width*layer->height);

		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int i = (y*layer->width) + x;

				int tile_id = layer->Get(x, y);
				TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileId(tile_id) : NULL;

				if (tileset != NULL)
				{
					//map[i] = (tile_id - tileset->firstgid) > 0 ? 0 : 1;
					map[i] = (tile_id) > 0 ? 0 : 1;
				}
			}
		}

		*buffer = map;
		width = data.width;
		height = data.height;
		ret = true;

		break;
	}

	return ret;
}

void j1Map::EntityMovement(j1Entity* entity)
{
	BROFILER_CATEGORY("EntityMovement", Profiler::Color::Blue)
	iPoint current_tile = WorldToMap(entity->collider->rect.x, entity->collider->rect.y);
	iPoint next_tile = { 0,0 };

	uint height0_gid = 0, height1_gid = 0, height2_gid = 0;

	height2_gid = App->map->data.layers.begin()._Ptr->_Next->_Next->_Myval->Get(current_tile.x, current_tile.y);

	if (height2_gid == 0) {
		height1_gid = App->map->data.layers.begin()._Ptr->_Next->_Myval->Get(current_tile.x, current_tile.y);
		if (height1_gid == 0)
			height0_gid = App->map->data.layers.begin()._Ptr->_Myval->Get(current_tile.x, current_tile.y);
	}

	uint current_gid;
	uint tile_id = App->map->data.layers.begin()._Ptr->_Myval->Get(current_tile.x, current_tile.y);
	int sum = (tile_id % 2 != 0) ? 1 : 0;
	if (height2_gid != 0) {				//entity is on the third layer
		current_gid = height2_gid;
		entity->height = 4 + sum;
		entity->current_height = 2;
	}
	else if (height1_gid != 0) {		//entity is on the second layer
		current_gid = height1_gid;
		entity->height = 2 + sum;
		entity->current_height = 1;
	}
	else {				                //entity is on the first layer
		current_gid = height0_gid;
		entity->height = 0 + sum;
		entity->current_height = 0;
	}

	// tiles of the first layer | height == 0
	uint up_right_gid = App->map->data.layers.begin()._Ptr->_Myval->Get(current_tile.x, current_tile.y - 1);
	uint down_left_gid = App->map->data.layers.begin()._Ptr->_Myval->Get(current_tile.x, current_tile.y + 1);
	uint down_right_gid = App->map->data.layers.begin()._Ptr->_Myval->Get(current_tile.x + 1, current_tile.y);
	uint up_left_gid = App->map->data.layers.begin()._Ptr->_Myval->Get(current_tile.x - 1, current_tile.y);

	uint right_gid = App->map->data.layers.begin()._Ptr->_Myval->Get(current_tile.x + 1, current_tile.y - 1);
	uint up_gid = App->map->data.layers.begin()._Ptr->_Myval->Get(current_tile.x - 1, current_tile.y - 1);
	uint down_gid = App->map->data.layers.begin()._Ptr->_Myval->Get(current_tile.x + 1, current_tile.y + 1);
	uint left_gid = App->map->data.layers.begin()._Ptr->_Myval->Get(current_tile.x - 1, current_tile.y + 1);


	// tiles of the second layer | height == 1
	uint up_right_gid_1 = App->map->data.layers.begin()._Ptr->_Next->_Myval->Get(current_tile.x, current_tile.y - 1);
	uint down_left_gid_1 = App->map->data.layers.begin()._Ptr->_Next->_Myval->Get(current_tile.x, current_tile.y + 1);
	uint down_right_gid_1 = App->map->data.layers.begin()._Ptr->_Next->_Myval->Get(current_tile.x + 1, current_tile.y);
	uint up_left_gid_1 = App->map->data.layers.begin()._Ptr->_Next->_Myval->Get(current_tile.x - 1, current_tile.y);

	uint right_gid_1 = App->map->data.layers.begin()._Ptr->_Next->_Myval->Get(current_tile.x + 1, current_tile.y - 1);
	uint up_gid_1 = App->map->data.layers.begin()._Ptr->_Next->_Myval->Get(current_tile.x - 1, current_tile.y - 1);
	uint down_gid_1 = App->map->data.layers.begin()._Ptr->_Next->_Myval->Get(current_tile.x + 1, current_tile.y + 1);
	uint left_gid_1 = App->map->data.layers.begin()._Ptr->_Next->_Myval->Get(current_tile.x - 1, current_tile.y + 1);


	//tiles of the third layer | height = 2
	uint up_right_gid_2 = App->map->data.layers.begin()._Ptr->_Next->_Next->_Myval->Get(current_tile.x, current_tile.y - 1);
	uint down_left_gid_2 = App->map->data.layers.begin()._Ptr->_Next->_Next->_Myval->Get(current_tile.x, current_tile.y + 1);
	uint down_right_gid_2 = App->map->data.layers.begin()._Ptr->_Next->_Next->_Myval->Get(current_tile.x + 1, current_tile.y);
	uint up_left_gid_2 = App->map->data.layers.begin()._Ptr->_Next->_Next->_Myval->Get(current_tile.x - 1, current_tile.y);

	uint right_gid_2 = App->map->data.layers.begin()._Ptr->_Next->_Next->_Myval->Get(current_tile.x + 1, current_tile.y - 1);
	uint up_gid_2 = App->map->data.layers.begin()._Ptr->_Next->_Next->_Myval->Get(current_tile.x - 1, current_tile.y - 1);
	uint down_gid_2 = App->map->data.layers.begin()._Ptr->_Next->_Next->_Myval->Get(current_tile.x + 1, current_tile.y + 1);
	uint left_gid_2 = App->map->data.layers.begin()._Ptr->_Next->_Next->_Myval->Get(current_tile.x - 1, current_tile.y + 1);

	bool height0_semiblock = false, height1_semiblock = false, height2_semiblock = false;
	bool height0_next = false, height1_next = false, height2_next = false;

	DIRECTION direction;

	direction = entity->direction;

	if (current_gid != 0) {
		switch (direction)
		{
		case UP_:
			if (up_gid_2 != 0) {
				next_gid = up_gid_2;
				if (next_gid % 2 == 0)
					height2_semiblock = true;
				else
					height2_semiblock = false;
				height2_next = true;
				next_height = 2;
			}
			else if (up_gid_1 != 0) {
				next_gid = up_gid_1;
				if (next_gid % 2 == 0)
					height1_semiblock = true;
				else
					height1_semiblock = false;
				height1_next = true;
				next_height = 1;
			}
			else if (up_gid != 0) {
				next_gid = up_gid;
				if (next_gid % 2 == 0)
					height0_semiblock = true;
				else
					height0_semiblock = false;
				height0_next = true;
				next_height = 0;
			}
			else
				next_gid = 0;

			next_tile = { current_tile.x - 1, current_tile.y - 1 };
			break;
		case DOWN_:
			if (down_gid_2 != 0) {
				next_gid = down_gid_2;
				if (next_gid % 2 == 0)
					height2_semiblock = true;
				else
					height2_semiblock = false;
				height2_next = true;
				next_height = 2;
			}
			else if (down_gid_1 != 0) {
				next_gid = down_gid_1;
				if (next_gid % 2 == 0)
					height1_semiblock = true;
				else
					height1_semiblock = false;
				height1_next = true;
				next_height = 1;
			}
			else if (down_gid != 0) {
				next_gid = down_gid;
				if (next_gid % 2 == 0)
					height0_semiblock = true;
				else
					height0_semiblock = false;
				height0_next = true;
				next_height = 0;
			}
			else
				next_gid = 0;
			next_tile = { current_tile.x + 1, current_tile.y + 1 };
			break;
		case RIGHT_:
			if (right_gid_2 != 0) {
				next_gid = right_gid_2;
				if (next_gid % 2 == 0)
					height2_semiblock = true;
				else
					height2_semiblock = false;
				height2_next = true;
				next_height = 2;
			}
			else if (right_gid_1 != 0) {
				next_gid = right_gid_1;
				if (next_gid % 2 == 0)
					height1_semiblock = true;
				else
					height1_semiblock = false;
				height1_next = true;
				next_height = 1;
			}
			else if (right_gid != 0) {
				next_gid = right_gid;
				if (next_gid % 2 == 0)
					height0_semiblock = true;
				else
					height0_semiblock = false;
				height0_next = true;
				next_height = 0;
			}
			else
				next_gid = 0;
			next_tile = { current_tile.x + 1, current_tile.y - 1 };
			break;
		case LEFT_:
			if (left_gid_2 != 0) {
				next_gid = left_gid_2;
				if (next_gid % 2 == 0)
					height2_semiblock = true;
				else
					height2_semiblock = false;
				height2_next = true;
				next_height = 2;
			}
			else if (left_gid_1 != 0) {
				next_gid = left_gid_1;
				if (next_gid % 2 == 0)
					height1_semiblock = true;
				else
					height1_semiblock = false;
				height1_next = true;
				next_height = 1;
			}
			else if (left_gid != 0) {
				next_gid = left_gid;
				if (next_gid % 2 == 0)
					height0_semiblock = true;
				else
					height0_semiblock = false;
				height0_next = true;
				next_height = 0;
			}
			else
				next_gid = 0;
			next_tile = { current_tile.x - 1, current_tile.y + 1 };
			break;
		case UP_RIGHT_:
			if (up_right_gid_2 != 0) {
				next_gid = up_right_gid_2;
				if (next_gid % 2 == 0)
					height2_semiblock = true;
				else
					height2_semiblock = false;
				height2_next = true;
				next_height = 2;
			}
			else if (up_right_gid_1 != 0) {
				next_gid = up_right_gid_1;
				if (next_gid % 2 == 0)
					height1_semiblock = true;
				else
					height1_semiblock = false;
				height1_next = true;
				next_height = 1;
			}
			else if (up_right_gid != 0) {
				next_gid = up_right_gid;
				if (next_gid % 2 == 0)
					height0_semiblock = true;
				else
					height0_semiblock = false;
				height0_next = true;
				next_height = 0;
			}
			else
				next_gid = 0;
			next_tile = { current_tile.x, current_tile.y - 1 };
			break;
		case UP_LEFT_:
			if (up_left_gid_2 != 0) {
				next_gid = up_left_gid_2;
				if (next_gid % 2 == 0)
					height2_semiblock = true;
				else
					height2_semiblock = false;
				height2_next = true;
				next_height = 2;
			}
			else if (up_left_gid_1 != 0) {
				next_gid = up_left_gid_1;
				if (next_gid % 2 == 0)
					height1_semiblock = true;
				else
					height1_semiblock = false;
				height1_next = true;
				next_height = 1;
			}
			else if (up_left_gid != 0) {
				next_gid = up_left_gid;
				if (next_gid % 2 == 0)
					height0_semiblock = true;
				else
					height0_semiblock = false;
				height0_next = true;
				next_height = 0;
			}
			else
				next_gid = 0;
			next_tile = { current_tile.x - 1, current_tile.y };
			break;
		case DOWN_RIGHT_:
			if (down_right_gid_2 != 0) {
				next_gid = down_right_gid_2;
				if (next_gid % 2 == 0)
					height2_semiblock = true;
				else
					height2_semiblock = false;
				height2_next = true;
				next_height = 2;
			}
			else if (down_right_gid_1 != 0) {
				next_gid = down_right_gid_1;
				if (next_gid % 2 == 0)
					height1_semiblock = true;
				else
					height1_semiblock = false;
				height1_next = true;
				next_height = 1;
			}
			else if (down_right_gid != 0) {
				next_gid = down_right_gid;
				if (next_gid % 2 == 0)
					height0_semiblock = true;
				else
					height0_semiblock = false;
				height0_next = true;
				next_height = 0;
			}
			else
				next_gid = 0;
			next_tile = { current_tile.x + 1, current_tile.y };
			break;
		case DOWN_LEFT_:
			if (down_left_gid_2 != 0) {
				next_gid = down_left_gid_2;
				if (next_gid % 2 == 0)
					height2_semiblock = true;
				else
					height2_semiblock = false;
				height2_next = true;
				next_height = 2;
			}
			else if (down_left_gid_1 != 0) {
				next_gid = down_left_gid_1;
				if (next_gid % 2 == 0)
					height1_semiblock = true;
				else
					height1_semiblock = false;
				height1_next = true;
				next_height = 1;
			}
			else if (down_left_gid != 0) {
				next_gid = down_left_gid;
				if (next_gid % 2 == 0)
					height0_semiblock = true;
				else
					height0_semiblock = false;
				height0_next = true;
				next_height = 0;
			}
			else
				next_gid = 0;
			next_tile = { current_tile.x, current_tile.y + 1 };
			break;
		}
	}

	if (((next_gid % 2) - (current_gid % 2) == 0 && (entity->current_height != next_height)) || next_gid == 0 || next_gid == 5 || next_gid == 6)
		entity->movement = false;

	else {
		entity->movement = true;

		if (((next_gid % 2) - (current_gid % 2)) == 1 || ((next_gid % 2) - (current_gid % 2)) == -1
			|| ((next_gid % 2) == 0 && (current_gid % 2) == 0) && next_gid != 0 && direction != NONE_ && current_gid != next_gid) {

			if ((current_gid == height0_gid && !height0_semiblock && current_gid % 2 == 0 && height0_next) ||
				(current_gid == height0_gid && height1_semiblock && current_gid % 2 == 1 && height1_next) ||
				(current_gid == height1_gid && !height1_semiblock && current_gid % 2 == 0 && height1_next) ||
				(current_gid == height1_gid && height2_semiblock && current_gid % 2 == 1 && height2_next) ||
				(current_gid == height2_gid && !height2_semiblock && current_gid % 2 == 0 && height2_next)) {
				if (((current_tile.x - next_tile.x == 1) && (current_tile.y - next_tile.y == -1) && direction == LEFT_
					&& (((down_left_gid_1 != next_gid && height1_next) || (down_left_gid_2 != next_gid && height2_next)) ||
					((left_gid_1 != next_gid && height1_next) || (left_gid_2 != next_gid && height2_next))))
					|| ((current_tile.x - next_tile.x == 1) && (current_tile.y - next_tile.y == 1) && direction == UP_)
					|| ((current_tile.x - next_tile.x == 1) && (current_tile.y - next_tile.y == 0) && direction == UP_LEFT_))
					entity->position.y -= 1;
				else if (((current_tile.x - next_tile.x == -1) && (current_tile.y - next_tile.y == 1) && direction == RIGHT_
					&& (((down_right_gid_1 != next_gid && height1_next) || (down_right_gid_2 != next_gid && height2_next)) ||
					((down_gid_1 != next_gid && height1_next) || (down_gid_2 != next_gid && height2_next))))
					|| ((current_tile.x - next_tile.x == 1) && (current_tile.y - next_tile.y == 1) && direction == UP_)
					|| ((current_tile.x - next_tile.x == 0) && (current_tile.y - next_tile.y == 1) && direction == UP_RIGHT_))
					entity->position.y -= 1;
				else if (((current_tile.x - next_tile.x == -1) && (current_tile.y - next_tile.y == 1) && direction == RIGHT_)
					|| ((current_tile.x - next_tile.x == -1) && (current_tile.y - next_tile.y == -1) && direction == DOWN_)
					&& (((down_right_gid_1 == next_gid && height1_next) || (down_right_gid_2 == next_gid && height2_next)) ||
					((right_gid_1 == next_gid && height1_next) || (right_gid_2 == next_gid && height2_next)))
					|| ((current_tile.x - next_tile.x == -1) && (current_tile.y - next_tile.y == 0) && direction == DOWN_RIGHT_)) {
					entity->position.y += 1;
					entity->position.x += 1;
				}
				else if (((current_tile.x - next_tile.x == 1) && (current_tile.y - next_tile.y == -1) && direction == LEFT_)
					|| ((current_tile.x - next_tile.x == -1) && (current_tile.y - next_tile.y == -1) && direction == DOWN_)
					|| ((current_tile.x - next_tile.x == 0) && (current_tile.y - next_tile.y == -1) && direction == DOWN_LEFT_)) {
					entity->position.y += 1;
					entity->position.x -= 1;
				}
			}

			else if ((current_gid == height2_gid && height2_semiblock && current_gid % 2 == 1 && height2_next) ||
				(current_gid == height2_gid && !height1_semiblock && current_gid % 2 == 0 && height1_next) ||
				(current_gid == height1_gid && height1_semiblock && current_gid % 2 == 1 && height1_next) ||
				(current_gid == height1_gid && !height0_semiblock && current_gid % 2 == 0 && height0_next) ||
				(current_gid == height0_gid && height0_semiblock && current_gid % 2 == 1 && height0_next)) {
				if (((current_tile.x - next_tile.x == -1) && (current_tile.y - next_tile.y == 1) && direction == RIGHT_
					&& (((up_right_gid_2 != next_gid && height2_next && up_right_gid_2 != 0) || (up_right_gid_2 != next_gid && height1_next && up_right_gid_2 != 0)) ||
					((up_right_gid_1 != next_gid && height1_next && up_right_gid_1 != 0) || (up_right_gid_1 != next_gid && height0_next && up_right_gid_1 != 0) ||
						(up_right_gid != next_gid && height0_next && up_right_gid != 0)) || ((up_gid_2 != next_gid && height2_next && up_gid_2 != 0) ||
						(up_gid_2 != next_gid && height1_next && up_gid_2 != 0)) || ((up_gid_1 != next_gid && height1_next && up_gid_1 != 0) ||
							(up_gid_1 != next_gid && height0_next && up_gid_1 != 0) || (up_gid != next_gid && height0_next && up_gid != 0))))
					|| ((current_tile.x - next_tile.x == -1) && (current_tile.y - next_tile.y == -1) && direction == DOWN_)
					|| ((current_tile.x - next_tile.x == -1) && (current_tile.y - next_tile.y == 0) && direction == DOWN_RIGHT_))
					entity->position.y += 1;
				else if (((current_tile.x - next_tile.x == 1) && (current_tile.y - next_tile.y == -1) && direction == LEFT_
					&& (((up_left_gid_2 != next_gid && height2_next && up_left_gid_2 != 0) || (up_left_gid_2 != next_gid && height1_next && up_left_gid_2 != 0)) ||
					((up_left_gid_1 != next_gid && height1_next && up_left_gid_1 != 0) || (up_left_gid_1 != next_gid && height0_next && up_left_gid_1 != 0) ||
						(up_left_gid != next_gid && height0_next && up_left_gid != 0)) || ((up_gid_2 != next_gid && height2_next && up_gid_2 != 0) ||
						(up_gid_2 != next_gid && height1_next && up_gid_2 != 0)) || ((up_gid_1 != next_gid && height1_next && up_gid_1 != 0) ||
							(up_gid_1 != next_gid && height0_next && up_gid_1 != 0) || (up_gid != next_gid && height0_next && up_gid != 0))))
					|| ((current_tile.x - next_tile.x == -1) && (current_tile.y - next_tile.y == -1) && direction == DOWN_)
					|| ((current_tile.x - next_tile.x == 0) && (current_tile.y - next_tile.y == -1) && direction == DOWN_LEFT_))
					entity->position.y += 1;
				else if (((current_tile.x - next_tile.x == -1) && (current_tile.y - next_tile.y == 1) && direction == RIGHT_)
					|| ((current_tile.x - next_tile.x == 1) && (current_tile.y - next_tile.y == 1) && direction == UP_
						&& (((up_right_gid_2 == next_gid && height2_next) || (up_right_gid_2 == next_gid && height1_next)) ||
						((up_right_gid_1 == next_gid && height1_next) || (up_right_gid_1 == next_gid && height0_next) ||
							(up_right_gid == next_gid && height0_next)) || ((right_gid_2 == next_gid && height2_next) ||
							(right_gid_2 == next_gid && height1_next)) || ((right_gid_1 == next_gid && height1_next) ||
								(right_gid_1 == next_gid && height0_next) || (right_gid == next_gid && height0_next))))
					|| ((current_tile.x - next_tile.x == 0) && (current_tile.y - next_tile.y == 1) && direction == UP_RIGHT_)) {
					entity->position.x += 1;
				}
				else if (((current_tile.x - next_tile.x == 1) && (current_tile.y - next_tile.y == -1) && direction == LEFT_)
					|| ((current_tile.x - next_tile.x == 1) && (current_tile.y - next_tile.y == 1) && direction == UP_)
					|| ((current_tile.x - next_tile.x == 1) && (current_tile.y - next_tile.y == 0) && direction == UP_LEFT_)) {
					entity->position.x -= 1;
				}
			}
		}
	}
}

void j1Map::Tile_WorldMap(iPoint& pos, int height){	
	int re2 = 0;
	if (height < 2) {
		re2 = 0;
	}
	else if (height < 4 && height >= 2)
		re2 = 1;
	else if (height < 6 && height >= 4) {
		re2 = 2;
	}
	else if (height < 8 && height >= 6) {
		re2 = 3;
	}
	pos.x += re2;
	pos.y += re2;
}

void j1Map::Entity_WorldMap(iPoint& pos, int height) {

	iPoint re1 = { 0,0 };
	if (height < 2) {
		re1.x = 1;
		re1.y = (((int)height + 1) % 2 == 0) ? 1 : 0;
	}
	else if (height < 4 && height >= 2) {
		re1.x = (((int)height + 1) % 2 == 0) ? 2 : 1;
		re1.y = (((int)height + 1) % 2 == 0) ? 2 : 1;
	}
	else if (height < 6 && height >= 4) {
		re1.x = (((int)height + 1) % 2 == 0) ? 3 : 2;
		re1.y = (((int)height + 1) % 2 == 0) ? 3 : 2;
	}
	else if (height < 8 && height >= 6) {
		re1.x = (((int)height + 1) % 2 == 0) ? 4 : 3;
		re1.y = (((int)height + 1) % 2 == 0) ? 4 : 3;
	}

	pos.x += height - re1.x;
	pos.y += height - re1.y;
}
