#ifndef __j1SCENELOSE_H__
#define __j1SCENELOSE_H__

#include "j1Module.h"
#include "j1Timer.h"
#include <list>

enum SCENE;
struct SDL_Texture;
struct _TTF_Font;
class j1Button;
class j1Label;

class j1SceneLose : public j1Module
{
public:

	j1SceneLose();

	// Destructor
	virtual ~j1SceneLose();

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
	void ChangeScene(SCENE objectiveScene);

public:

	std::list<j1Button*> deathButtons;
	std::list<j1Label*> deathLabels;

	j1Timer	startup_time;

	bool continueGame = true;
	bool backToMenu = false;
	bool startGame = false;

	uint times = 0;

private:

	SDL_Texture* gui_tex2 = nullptr;
	_TTF_Font* font = nullptr;
	_TTF_Font* font2 = nullptr;
	_TTF_Font* font3 = nullptr;

};

#endif // __j1SCENELOSE_H__
