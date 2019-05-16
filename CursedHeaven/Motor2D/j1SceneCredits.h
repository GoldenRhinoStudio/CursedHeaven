#ifndef __j1SCENECREDITS_H__
#define __j1SCENECREDITS_H__

#include "j1Module.h"
#include "j1Timer.h"
#include <list>

struct SDL_Texture;
class j1Button;
class j1Label;

class j1SceneCredits : public j1Module
{
public:

	j1SceneCredits();

	// Destructor
	virtual ~j1SceneCredits();

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
	std::list<j1Button*> creditsButtons;
	std::list<j1Label*> creditsLabels;
	std::list<j1Box*> creditsBoxes;

	j1Timer	startup_time;

	j1Box* credits_window = nullptr;

private:

	SDL_Texture* gui_tex2 = nullptr;
	SDL_Texture* license = nullptr;

	bool continueGame = true;
	bool backToMenu = false;
};

#endif // __j1SCENECREDITS_H__