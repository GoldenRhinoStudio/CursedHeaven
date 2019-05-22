#ifndef __j1SCENEKEYCONFIG_H__
#define __j1SCENEKEYCONFIG_H__

#include "j1Module.h"
#include "j1Render.h"
#include "j1Timer.h"
#include "p2Point.h"
#include "j1Input.h"
#include <list>

struct SDL_Texture;
struct _TTF_Font;
class j1Button;
class j1Label;
class j1Box;

class j1SceneKeyConfig : public j1Module {

public:

	j1SceneKeyConfig();

	virtual ~j1SceneKeyConfig();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

public:

	std::list<j1Label*> key_labels;
	std::list<j1Box*> key_boxes;
	std::list<j1Button*> key_buttons;

	j1Timer startup_time;

	int ChangeKey(int key_to_change);

	int MOVE_UP = SDL_SCANCODE_W;
	int MOVE_DOWN = SDL_SCANCODE_S;
	int MOVE_LEFT = SDL_SCANCODE_A;
	int MOVE_RIGHT = SDL_SCANCODE_D;
	int NORMAL_ATTACK = 1;
	int ABILITY1 = SDL_SCANCODE_Q;
	int ABILITY2 = SDL_SCANCODE_E;
	int BUY_ITEM = SDL_SCANCODE_RETURN;
	int USE_POTION = SDL_SCANCODE_R;
	int TABULADOR = SDL_SCANCODE_TAB;

private:

	bool continueGame = true;
	SDL_Texture* key_tex = nullptr;
	bool waiting = false;

	j1Timer change_key_timer;
};
#endif // !__j1SCENEKEYCONFIG_H__
