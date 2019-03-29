#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include <list>
#include <string>
#include "j1Module.h"
#include "j1Entity.h"
#include "TileQuadtree.h"

struct Properties
{
	struct Property
	{
		std::string  name;
		int value;
	};

	~Properties()
	{
		for (std::list<Property*>::const_iterator item = properties.begin(); item != properties.end(); ++item)
		{
			delete *item;
		}
			   
		properties.clear();
	}

	int Get(const char* name, int default_value = 0) const;

	std::list<Property*>	properties;
};

struct MapLayer
{
	std::string		name;
	int				width;
	int				height;
	TileQuadtree*	tile_tree;
	uint*			data;
	Properties		properties;

	MapLayer() : data(NULL)
	{}

	~MapLayer()
	{
		RELEASE(data);
	}

	// Short function to get the value of x,y
	inline uint Get(int x, int y) const
	{
		return data[(y*width) + x];
	}
};

// ----------------------------------------------------
struct TileSet
{
	SDL_Rect GetTileRect(int id) const;

	std::string			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture = nullptr;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	int						width;
	int						height;
	int						tile_width;
	int						tile_height;
	SDL_Color				background_color;
	MapTypes				type;
	std::list<TileSet*>		tilesets;
	std::list<MapLayer*>	layers;
};

// ----------------------------------------------------
class j1Map : public j1Module
{
	friend TileQuadtree;

public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	// Coordinate translation methods
	iPoint MapToWorld(int x, int y) const;
	iPoint WorldToMap(int x, int y) const;
	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;

	void EntityMovement(j1Entity* entity);

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);
	bool PutColliders(const char* file_name);

	TileSet* GetTilesetFromTileId(int id) const;


public:

	MapData data;

	bool draw_with_quadtrees;
	//TEST
	uint tiles_rendered;

	int current_height = 0;

private:

	float bgBlitSpeed;
	float fogBlitSpeed;
	float mapBlitSpeed;

	pugi::xml_document	map_file;
	std::string			folder;
	bool				map_loaded;
};

#endif // __j1MAP_H__