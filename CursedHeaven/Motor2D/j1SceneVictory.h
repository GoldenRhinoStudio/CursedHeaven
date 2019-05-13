#ifndef __j1SCENEVICTORY_H__
#define __j1SCENEVICTORY_H__

#include "j1Module.h"
#include "j1Timer.h"
#include <list>

enum SCENE;
struct SDL_Texture;
struct _TTF_Font;
class j1Button;
class j1Label;

class j1SceneVictory : public j1Module
{
public:

	j1SceneVictory();

	// Destructor
	virtual ~j1SceneVictory();

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

	std::list<j1Button*> victoryButtons;
	std::list<j1Label*> victoryLabels;

	j1Timer	startup_time;

	bool continueGame = true;
	bool backToMenu = false;
	bool startGame = false;

	uint times = 0;

private:

	SDL_Texture* gui_tex2 = nullptr;

};

#endif // __j1SCENEVICTORY_H__

