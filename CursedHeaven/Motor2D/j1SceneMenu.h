#ifndef __j1SCENEMENU_H__
#define __j1SCENEMENU_H__

#include "j1Module.h"
#include "j1Render.h"
#include "j1Timer.h"
#include <list>
#include "p2Point.h"
#include "p2Animation.h"

struct SDL_Texture;
struct _TTF_Font;
class j1Box;
class j1Button;
class j1Label;

enum SCENE {
	MENU = 0,
	CHOOSE,
	SCENE1,
	SCENE2,
	CREDITS,
	SCENE_SETTINGS,
	VICTORY,
	LOSE,
	VIDEO,
	KEY_CHANGES
};

class j1SceneMenu : public j1Module
{
public:
	j1SceneMenu();

	virtual ~j1SceneMenu();

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

	// Load and Save
	bool Load(pugi::xml_node&);
	//bool Save(pugi::xml_node&) const;

public:
	j1Box* settings_window = nullptr;

	std::list<j1Button*> menuButtons; 
	std::list<j1Label*> menuLabels;
	std::list<j1Box*> menuBoxes;

	j1Timer	startup_time;

	bool continueGame = true;
	bool loadGame = false;
	bool openCredits = false;
	bool player_created = false;
	bool chooseChar = false;
	bool openSettings = false;

private:

	SDL_Texture* gui_tex2 = nullptr;
	SDL_Texture* logo_tex = nullptr;

	_TTF_Font* font = nullptr;

	uint times = 0;

	SCENE currentScene = SCENE::MENU;
};

#endif //__j1SCENEMENU_H__