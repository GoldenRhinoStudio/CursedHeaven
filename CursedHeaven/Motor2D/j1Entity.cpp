#include "j1Entity.h"
#include "j1App.h"
#include "j1Render.h"

j1Entity::j1Entity(int x, int y, ENTITY_TYPES type) : position(x,y), type(type)
{
}

void j1Entity::Draw(SDL_Rect* r, bool flip, int x, int y)
{
	TileData* img = new TileData(0, position.x, position.y, 0, height,sprites,r,flip,collider);
	App->render->entities_sprites.push_back(img);

	/*
	if (flip == false)
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, &r, SDL_FLIP_NONE);
	else 
		App->render->Blit(sprites, (int)position.x + x, (int)position.y + y, &r, SDL_FLIP_HORIZONTAL);*/
}