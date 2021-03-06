#ifndef __j1CHOOSECHARACTER_H__
#define __j1CHOOSECHARACTER_H__

#include "j1Module.h"
#include "j1Button.h"
#include "j1Timer.h"
#include "j1Player.h"
#include "PugiXml/src/pugixml.hpp"
#include <list>

struct SDL_Texture;
struct _TTF_Font;
class j1Box;
class j1Button;
class j1Label;

class j1ChooseCharacter : public j1Module {

public:

	j1ChooseCharacter();

	virtual ~j1ChooseCharacter();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

public:

	j1Box* info_window = nullptr;

	std::list<j1Button*> chooseCharacterButtons;
	std::list<j1Label*> chooseCharacterLabels;
	std::list<j1Box*> chooseCharacterBoxes;
	
	j1Timer	startup_time;
	bool startGame = false;
	bool backToMenu = false;

private:
	SDL_Texture* button_tex = nullptr;
	SDL_Texture* gui_tex2 = nullptr;
	SDL_Texture* info_tex = nullptr;

	SCENE currentScene = SCENE::CHOOSE;
};

#endif