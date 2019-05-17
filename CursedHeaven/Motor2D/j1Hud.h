#ifndef __j1HUD_H__
#define __j1HUD_H__

#include "j1Timer.h"
#include <string>
#include <list>
#include "PugiXml/src/pugixml.hpp"
#include "p2Animation.h"

class j1Button;
class j1Label;

struct _TTF_Font;
struct SDL_Texture;

class j1Hud
{
public:
	j1Hud();
	~j1Hud();

	bool Start();
	bool Update(float dt);
	bool CleanUp();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

public:
	bool black_mage = false;
	bool dragoon_knight = false;

	float life_points_max, life_points;
	char const *current_points;

private:
	SDL_Texture* hud_text = nullptr;
	SDL_Texture* profile_text = nullptr;
	SDL_Texture* score = nullptr;
	SDL_Texture* dialog_tex = nullptr;

	_TTF_Font* font = nullptr;

	std::list<j1Label*> labels_list;
	std::list<j1Button*> hud_buttons;

	int pivot = 2147483647;
	float multiplier;
};

#endif // __j1LHUD_H__