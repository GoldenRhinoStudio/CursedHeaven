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

	SDL_Rect idle5 = { 382, 508, 37, 36 };
	SDL_Rect hovered5 = { 343, 508, 37, 36 };
	SDL_Rect clicked5 = { 421, 508, 37, 36 };
	SDL_Rect idle = { 631, 12, 151, 38 };
	SDL_Rect hovered = { 963, 12, 151, 38 };
	SDL_Rect clicked = { 797, 12, 151, 38 };

	int MOVE_UP = SDL_SCANCODE_W;
	int MOVE_DOWN = SDL_SCANCODE_S;
	int MOVE_LEFT = SDL_SCANCODE_A;
	int MOVE_RIGHT = SDL_SCANCODE_D;
	int ABILITY1 = SDL_SCANCODE_Q;
	int ABILITY2 = SDL_SCANCODE_E;
	int BUY_ITEM = SDL_SCANCODE_RETURN;
	int USE_POTION = SDL_SCANCODE_R;
	int TABULADOR = SDL_SCANCODE_TAB;

	char text[20];
	char text2[20];
	char text3[20];
	char text4[20];
	char text5[20];
	char text6[20];
	char text7[20];
	char text8[20];
	char text9[20];
	char text10[20];
	char text11[20];
	char text12[20];
	char text13[20];
	char text14[20];
	char text15[20];
	char text16[20];
	char text17[20];
	char text18[20];
	char text19[20];
	char text20[20];
	char text21[20];
	char text22[20];
	char text23[20];
	char text24[20];
	char text25[20];
	char text26[20];
	char text27[20];
	char text28[20];
	char text29[20];
	char text30[20];
	char text31[20];
	char text32[20];
	char text33[20];


	const char* move_up = nullptr;
	const char* move_down = nullptr;
	const char* move_right = nullptr;
	const char* move_left = nullptr;
	const char* ability1_text = nullptr;
	const char* ability2_text = nullptr;
	const char* item_text = nullptr;
	const char* potion_text = nullptr;
	const char* stats_text = nullptr;

	int last_changed = -1;

	bool waiting = false;

private:

	j1Label* up;
	j1Label* down;
	j1Label* right;
	j1Label* left;
	j1Label* stats;
	j1Label* ability1;
	j1Label* ability2;
	j1Label* item;
	j1Label* potion;

	bool continueGame = true;
	SDL_Texture* key_tex = nullptr;

	j1Timer change_key_timer;
};
#endif // !__j1SCENEKEYCONFIG_H__
