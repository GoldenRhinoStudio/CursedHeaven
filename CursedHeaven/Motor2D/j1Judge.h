#ifndef __j1JUDGE_H__
#define __j1JUDGE_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2Point.h"
#include "p2Animation.h"
#include "j1Entity.h"

struct SDL_Texture;

enum LawType {
	Q_ABILITY,
	E_ABILITY,
	EVERY_ROOM,
	HEALTH
};

class j1Judge : public j1Entity
{

public:
	j1Judge(int x, int y, ENTITY_TYPES type);
	~j1Judge();

	bool Start();
	bool PreUpdate() { return true; };
	bool Update(float dt, bool do_logic);
	bool DrawOrder(float dt);
	bool PostUpdate() { return true; };
	bool CleanUp() { return true; };

	// Load / Save
	bool Load(pugi::xml_node&) { return true; };
	bool Save(pugi::xml_node&) const { return true; };


private:
	Animation idle;

};

#endif // __j1JUDGE_H__
