#ifndef __j1SCENE2_H__
#define __j1SCENE2_H__

#include "j1Module.h"
#include "j1Timer.h"

struct SDL_Texture;
class j1Box;

class j1Scene2 : public j1Module
{
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

	// Called to change scene
	void ChangeScene();
	void ChangeSceneMenu();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	void PlaceEntities();

public:
	j1Box* settings_window = nullptr;
	fPoint initialScene2Position;

	p2List<j1Button*> scene2Buttons;
	p2List<j1Label*> scene2Labels;
	p2List<j1Box*> scene2Boxes;

	bool backToMenu = false;
	bool changingScene = false;

	j1Timer	startup_time;
	int time_scene2;

private:

	bool player_created = false;
	bool continueGame = true;

	Animation coin_hud;
	SDL_Texture* coin_tex = nullptr;
	SDL_Texture* debug_tex = nullptr;
	SDL_Texture* gui_tex = nullptr;

	_TTF_Font* font = nullptr;

	bool closeSettings = false;
	bool resettingLevel = false;

};

#endif // __j1SCENE2_H__