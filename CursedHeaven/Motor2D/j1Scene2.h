#ifndef __j1SCENE2_H__
#define __j1SCENE2_H__

#include "j1Module.h"
#include "j1Button.h"
#include "j1Timer.h"
#include "PugiXml/src/pugixml.hpp"
#include <list>

struct SDL_Texture;
struct _TTF_Font;
class j1Box;

class j1Scene2 : public j1Module {
public:

	j1Scene2();

	// Destructor
	virtual ~j1Scene2();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	//// Called to change scene
	//void ChangeSceneMenu();
	//void ChangeSceneDeath();
	//void ChangeSceneVictory();

	// Load and Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	void PlaceEntities(int room);

private:
	SDL_Texture * debug_tex = nullptr;
	SDL_Texture* gui_tex = nullptr;

	SDL_Texture* lvl2_tex = nullptr;

	SDL_Rect rect = { 0,0,400,200 };

	_TTF_Font* font = nullptr;
};

#endif