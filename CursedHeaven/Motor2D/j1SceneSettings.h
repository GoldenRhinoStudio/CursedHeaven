#ifndef __j1SCENESETTINGS_H__
#define __j1SCENESETTINGS_H__

#include "j1Module.h"
#include "j1Render.h"
#include "j1Timer.h"
#include <list>
#include "p2Point.h"
#include "p2Animation.h"

struct SDL_Texture;
struct _TTF_Font;
class j1Button;
class j1Label;
class j1Box;

class j1SceneSettings : public j1Module
{
public:

	j1SceneSettings();

	// Destructor
	virtual ~j1SceneSettings();

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

	// Called to change scene
	void ChangeScene();

public:
	std::list<j1Label*> setsLabels;
	std::list<j1Box*> setsBoxes;
	std::list<j1Button*> setsButtons;

	j1Timer	startup_time;

	j1Box* sets_window = nullptr;

	bool continueGame = true;
	bool backToMenu = false;

	

	uint times = 0;

private:

	_TTF_Font* font3 = nullptr;
	SDL_Texture* gui_tex = nullptr;
	SDL_Texture* gui_tex2 = nullptr;

	
};

#endif // __j1SCENESETTINGS	_H__
