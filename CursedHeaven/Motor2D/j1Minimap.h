#include "SDL\include\SDL.h"
#include "j1App.h"
#include "j1Module.h"


struct SDL_Texture;
struct SDL_Rect;

class j1Minimap : public j1Module
{
public:
	j1Minimap();
	virtual ~j1Minimap();

	bool Awake(pugi::xml_node& config);

	bool Start();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

public:

	bool MinimapCoords(int& map_x, int& map_y);
	void DrawCamera();
	void MinimapBorders();
	void DrawEntities();

private:

	SDL_Texture* lvl1_tex = nullptr;
	SDL_Rect rect = { 0,0,691,341 };
	
	int minimap_width;
	int minimap_height;

};