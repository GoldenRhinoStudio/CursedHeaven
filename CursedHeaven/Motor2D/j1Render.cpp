#include "p2Defs.h"
#include "p2Log.h"
#include "j1Window.h"
#include "j1Render.h"
#include "j1EntityManager.h"
#include "j1Map.h"
#include "j1Player.h"
#include "j1Collisions.h"

#include "Brofiler/Brofiler.h"

j1Render::j1Render() : j1Module()
{
	name.assign("renderer");
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;

}

// Destructor
j1Render::~j1Render()
{}

// Called before render is available
bool j1Render::Awake(pugi::xml_node& config)
{
	LOG("Create SDL rendering context");
	bool ret = true;
	// load flags
	Uint32 flags = SDL_RENDERER_ACCELERATED;

	if (config.child("vsync").attribute("value").as_bool(true) == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
		LOG("Using vsync");
		usingVsync = true;
	}
	else
		usingVsync = false;

	renderer = SDL_CreateRenderer(App->win->window, -1, flags);

	if (renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = App->win->screen_surface->w;
		camera.h = App->win->screen_surface->h;
		camera.x = initialCameraX = 0;
		camera.y = initialCameraY = 0;
	}

	return ret;
}

// Called before the first frame
bool j1Render::Start()
{
	LOG("render start");

	//SDL_RenderSetLogicalSize(renderer, App->win->width, App->win->height); //LIMIT SCREEN VIEW
	SDL_RenderGetViewport(renderer, &viewport);
	return true;
}

// Called each loop iteration
bool j1Render::PreUpdate()
{
	BROFILER_CATEGORY("RendererPreUpdate", Profiler::Color::Orange)


		while (!map_sprites.empty())
		{
			map_sprites.pop_back();
		}
	map_sprites.clear();

	while (!entities_sprites.empty())
	{
		entities_sprites.pop_back();
	}
	entities_sprites.clear();


	SDL_RenderClear(renderer);
	return true;
}

bool j1Render::Update(float dt)
{
	BROFILER_CATEGORY("RendererUpdate", Profiler::Color::LightSeaGreen)

		j1Player* player = nullptr;

	if (App->entity->knight != nullptr) player = (j1Player*)App->entity->knight;
	else if (App->entity->mage != nullptr) player = (j1Player*)App->entity->mage;
	/*else if (App->entity->rogue != nullptr) player = (j1Player*)App->entity->rogue;
	else if (App->entity->tank != nullptr) (j1Player*)App->entity->tank;*/

	if (player != nullptr) {

		if (!player->changing_room) {
			camera.x = -player->position.x * (App->win->GetScale()) + App->win->width / 2;
			camera.y = -player->position.y * (App->win->GetScale()) + App->win->height / 2;
		}

		if (player->changing_room == true) {
			if (camera.x < -player->position.x * (App->win->GetScale()) + App->win->width / 2) {
				camera.x += 250 * dt;
			}
			else if (camera.x > -player->position.x * (App->win->GetScale()) + App->win->width / 2) {
				camera.x -= 250 * dt;
			}
			else player->changing_room = false;

			if (camera.y < -player->position.y * (App->win->GetScale()) + App->win->height / 2) {
				camera.y += 250 * dt;
			}
			else if (camera.y > -player->position.y * (App->win->GetScale()) + App->win->height / 2) {
				camera.y -= 250 * dt;
			}
			else player->changing_room = false;
		}
	}

	return true;
}

bool j1Render::PostUpdate()
{
	BROFILER_CATEGORY("RendererPostUpdate", Profiler::Color::Yellow)

		SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);
	SDL_RenderPresent(renderer);
	return true;
}

// Called before quitting
bool j1Render::CleanUp()
{
	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	return true;
}

// Load Game State
bool j1Render::Load(pugi::xml_node& data)
{
	camera.x = data.child("camera").attribute("x").as_int();
	camera.y = data.child("camera").attribute("y").as_int();

	return true;
}

// Save Game State
bool j1Render::Save(pugi::xml_node& data) const
{
	pugi::xml_node cam = data.append_child("camera");

	cam.append_attribute("x") = camera.x;
	cam.append_attribute("y") = camera.y;

	return true;
}

void j1Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void j1Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void j1Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

iPoint j1Render::ScreenToWorld(int x, int y) const
{
	iPoint ret;
	int scale = App->win->GetScale();

	ret.x = (x - camera.x / scale);
	ret.y = (y - camera.y / scale);

	return ret;
}

// Blit to screen
bool j1Render::Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section, SDL_RendererFlip flip, float speed, float blitScale, double angle, int pivot_x, int pivot_y, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();
	SDL_Rect rect;

	if (use_camera)
	{
		rect.x = (int)(camera.x * speed) + x * scale;
		rect.y = (int)(camera.y * speed) + y * scale;
	}
	else
	{
		rect.x = x * SCREEN_SIZE;
		rect.y = y * SCREEN_SIZE;
	}


	if (section != NULL)
	{
		rect.w = section->w;

		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	(int)rect.w *= scale * blitScale;
	(int)rect.h *= scale * blitScale;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if (pivot_x != INT_MAX && pivot_y != INT_MAX)
	{
		pivot.x = pivot_x;
		pivot.y = pivot_y;
		p = &pivot;
	}

	if (SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, flip) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Blit to screen
bool j1Render::BlitDialog(SDL_Texture* texture, int x, int y, const SDL_Rect* section, SDL_RendererFlip flip, float speed, float Scale, double angle, int pivot_x, int pivot_y) const
{
	bool ret = true;
	uint scale = Scale;

	SDL_Rect rect;
	rect.x = (int)(camera.x * speed) + x * scale;
	rect.y = (int)(camera.y * speed) + y * scale;

	if (section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= scale;
	rect.h *= scale;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if (pivot_x != INT_MAX && pivot_y != INT_MAX)
	{
		pivot.x = pivot_x;
		pivot.y = pivot_y;
		p = &pivot;
	}

	if (SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, flip) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}


bool j1Render::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if (use_camera)
	{
		rec.x = (int)(camera.x + rect.x * scale);
		rec.y = (int)(camera.y + rect.y * scale);
		rec.w *= scale;
		rec.h *= scale;
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	if (use_camera)
		result = SDL_RenderDrawLine(renderer, camera.x + x1 * scale, camera.y + y1 * scale, camera.x + x2 * scale, camera.y + y2 * scale);
	else
		result = SDL_RenderDrawLine(renderer, x1 * scale, y1 * scale, x2 * scale, y2 * scale);

	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_Point points[360];

	float factor = (float)M_PI / 180.0f;

	for (uint i = 0; i < 360; ++i)
	{
		points[i].x = (int)(x + radius * cos(i * factor));
		points[i].y = (int)(y + radius * sin(i * factor));
	}

	result = SDL_RenderDrawPoints(renderer, points, 360);

	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::OrderBlit(priority_queue<TileData*, vector<TileData*>, Comparer>& priority) const
{
	BROFILER_CATEGORY("OrderBlit", Profiler::Color::Gold)
		bool ret = true;
	while (priority.empty() == false) {
		TileData* Image = priority.top();

		uint size = App->win->GetScale();
		SDL_Rect r;
		r.x = (int)(camera.x * Image->speed) + Image->x * size;
		r.y = (int)(camera.y * Image->speed) + Image->y * size;

		if (Image->section != NULL) {
			r.w = Image->section->w;
			r.h = Image->section->h;
		}
		else {
			SDL_QueryTexture(Image->texture, NULL, NULL, &r.w, &r.h);
		}
		SDL_RendererFlip flag;
		if (Image->flip == true)
			flag = SDL_FLIP_HORIZONTAL;
		else
			flag = SDL_FLIP_NONE;

		(int)r.w *= Image->scale * Image->blitScale;
		(int)r.h *= Image->scale * Image->blitScale;

		SDL_Point* point = NULL;
		SDL_Point img2;

		if (Image->pivot_x != INT_MAX && Image->pivot_y != INT_MAX) {
			img2.x = Image->pivot_x;
			img2.y = Image->pivot_y;
			point = &img2;
		}
		if (SDL_RenderCopyEx(renderer, Image->texture, Image->section, &r, Image->angle, point, flag) != 0) {
			LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
			ret = false;
		}
		priority.pop();
	}
	for (int i = 0; i < App->map->Rectvec.size(); i++) {
		RELEASE(App->map->Rectvec[i]);
	}

	App->map->Rectvec.clear();
	return ret;
}

void j1Render::reOrder() {

	BROFILER_CATEGORY("Order", Profiler::Color::Green)
		bool behind = false;
	int aux_height = 0;
	int sum_height = 0;
	iPoint pos1 = { 0,0 };
	iPoint pos2 = { 0,0 };
	for (std::vector<TileData*>::iterator item_map = map_sprites.begin(); item_map != map_sprites.end(); ++item_map)
	{
		TileData* img2 = *item_map;
		pos2 = App->map->WorldToMap(img2->x, img2->y);
		App->map->Tile_WorldMap(pos2, img2->height);

		for (std::vector<TileData*>::iterator item = entities_sprites.begin(); item != entities_sprites.end(); ++item)
		{

			TileData* img1 = *item;
			pos1 = App->map->WorldToMap((int)img1->col->rect.x, (int)img1->col->rect.y + 12);
			App->map->Tile_WorldMap(pos1, img1->height);


			if (img1->height < img2->height && !img1->behind) {//check

				if ((pos2.x == pos1.x - 1 || pos2.x == pos1.x - 2 || pos2.x == pos1.x - 3) && (pos2.y == pos1.y - 1 || pos2.y == pos1.y - 2))//top-left
				{
					img1->order = img2->order + 0.5f;
				}
				else if ((pos2.y == pos1.y - 1) && pos2.x == pos1.x)//top
				{
					img1->order = img2->order + 0.5f;
				}
				else if ((pos2.y == pos1.y - 1) && (pos2.x == pos1.x + 1))//top-right
				{
					img1->order = img2->order + 0.5f;
				}
				else if ((pos2.x == pos1.x - 1 || pos2.x == pos1.x - 2) && pos2.y == pos1.y) //left
				{
					img1->order = img2->order + 0.5f;
				}
				else if (pos2.y == pos1.y && pos2.x == pos1.x)//current
				{
					img1->order = img2->order - 0.5f;
					img1->behind = true;
				}
				else if ((pos2.x == pos1.x + 1) && pos2.y == pos1.y)//right
				{
					img1->order = img2->order - 0.5f;
					img1->behind = true;
				}
				else if (pos2.y == pos1.y + 1 && pos2.x == pos1.x - 1)//bottom-left
				{
					img1->order = img2->order + 0.5f;
				}
				else if (pos2.x == pos1.x && (pos2.y == pos1.y + 1 || pos2.y == pos1.y + 2))//bottom
				{
					img1->order = img2->order - 0.5f;
					img1->behind = true;
				}
				else if ((pos2.x == pos1.x + 1 || pos2.x == pos1.x + 2 || pos2.x == pos1.x + 3) && (pos2.y == pos1.y + 1 || pos2.y == pos1.y + 2 || pos2.y == pos1.y + 3))//bottom-right
				{
					img1->order = img2->order - 0.5f;
					img1->behind = true;
				}

			}
			else if (img1->height >= img2->height && !img1->behind) {
				if ((pos2.x == pos1.x - 1 && pos2.y == pos1.y) || //left
					(pos2.x == pos1.x - 1 && pos2.y == pos1.y - 1) || //top-left
					(pos2.x == pos1.x && pos2.y == pos1.y - 1) ||//top
					(pos2.x == pos1.x + 1 && pos2.y == pos1.y - 1) ||//top-right
					(pos2.x == pos1.x + 1 && pos2.y == pos1.y) ||//right
					(pos2.x == pos1.x + 1 && pos2.y == pos1.y + 1) ||//top-down
					(pos2.x == pos1.x + 2 && pos2.y == pos1.y + 2) ||//down-right
					(pos2.x == pos1.x && pos2.y == pos1.y + 1) ||//down
					(pos2.x == pos1.x - 1 && pos2.y == pos1.y + 1) || //down-left
					(pos2.x == pos1.x && pos2.y == pos1.y)) //current
				{/*
					if (img2->id % 2 != 0)
						img1->order = img2->order + 1.0f;
					else*/
					if (img2->order > img1->order)
						img1->order = img2->order + 0.5f;
				}
			}
		}
		OrderToRender.push(img2);
	}
	if (!entities_sprites.empty()) {
		TileData* player = entities_sprites.front();

		for (std::vector<TileData*>::iterator item = entities_sprites.begin(); item != entities_sprites.end(); ++item)
		{
			TileData* img1 = *item;
			pos1 = App->map->WorldToMap(img1->x, img1->y);

			for (std::vector<TileData*>::iterator item2 = entities_sprites.begin(); item2 != entities_sprites.end(); ++item2)
			{
				TileData* img2 = *item2;
				pos2 = App->map->WorldToMap(img2->x, img2->y);

				if (img2 != img1) {
					if ((pos2.x == pos1.x - 1 && pos2.y == pos1.y) || //left
						(pos2.x == pos1.x - 1 && pos2.y == pos1.y - 1) || //top-left
						(pos2.x == pos1.x && pos2.y == pos1.y - 1) ||//top
						(pos2.x == pos1.x + 1 && pos2.y == pos1.y - 1) ||//top-right
						(pos2.x == pos1.x + 1 && pos2.y == pos1.y) ||//right
						(pos2.x == pos1.x + 1 && pos2.y == pos1.y + 1) ||//top-down
						(pos2.x == pos1.x + 2 && pos2.y == pos1.y + 2) ||//down-right
						(pos2.x == pos1.x && pos2.y == pos1.y + 1) ||//down
						(pos2.x == pos1.x - 1 && pos2.y == pos1.y + 1) || //down-left
						(pos2.x == pos1.x && pos2.y == pos1.y)) {
						if (img1->col->rect.y + img1->col->rect.h < img2->col->rect.y + img2->col->rect.h) {
							img1->order = img2->order - 0.2f;
						}
					}
				}
			}

			OrderToRender.push(img1);
		}
	}
}
