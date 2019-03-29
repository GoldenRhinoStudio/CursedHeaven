#ifndef __j1DRAGOONKNIGHT_H__
#define __j1DRAGOONKNIGHT_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2Point.h"
#include "p2Animation.h"
#include "j1Player.h"

struct SDL_Texture;
enum ENTITY_TYPES;

class j1DragoonKnight : public j1Player
{

public:
	j1DragoonKnight(int x, int y, ENTITY_TYPES type);

	// Destructor
	virtual ~j1DragoonKnight();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt, bool do_logic);
	bool PostUpdate();

	void LoadPlayerProperties();

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	// Called before quitting
	bool CleanUp();

public:

	// Animations of the player
	Animation idle;
	Animation run;
	Animation godmode;
	Animation attackRight;
	Animation attackLeft;
	Animation death;

	// Attack values

	// Sounds

};

#endif // __j1DRAGOONKNIGHT_H__