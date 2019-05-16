#ifndef __j1SELLER_H__
#define __j1SELLER_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2Point.h"
#include "p2Animation.h"
#include "j1Entity.h"

struct SDL_Texture;

class j1Seller : public j1Entity
{

public:
	j1Seller(int x, int y, ENTITY_TYPES type);
	~j1Seller();

	bool Start();
	bool Update(float dt, bool do_logic); 
	bool DrawOrder(float dt);
	bool CleanUp();

	// Load / Save
	bool Load(pugi::xml_node&) { return true; };
	bool Save(pugi::xml_node&) const { return true; };

private:
	Animation idle;

};

#endif // __j1SELLER_H__