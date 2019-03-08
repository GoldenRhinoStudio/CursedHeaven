#include "j1Hud.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1UserInterfaceElement.h"
#include "j1Label.h"
#include "j1Scene1.h"
#include "j1Fonts.h"
#include "j1Render.h"
#include "j1Player.h"
#include "j1Input.h"

j1Hud::j1Hud() 
{
	animation = NULL;
	sprites = nullptr;
}

j1Hud::~j1Hud() {}

bool j1Hud::Start()
{
	return true;
}

bool j1Hud::Update(float dt)
{
	return true;
}

bool j1Hud::CleanUp()
{
	for (std::list<j1Label*>::iterator item = labels_list.begin(); item != labels_list.end(); ++item)
	{
		labels_list.remove(*item);
	}

	return true;
}

bool j1Hud::Load(pugi::xml_node & data)
{
	return true;
}

bool j1Hud::Save(pugi::xml_node & data) const
{
	return true;
}
