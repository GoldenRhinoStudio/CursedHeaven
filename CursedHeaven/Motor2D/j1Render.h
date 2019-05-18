#ifndef __j1RENDER_H__
#define __j1RENDER_H__

#include "SDL/include/SDL.h"
#include "p2Point.h"
#include "j1Module.h"
#include <queue>
#include <vector>
#include "TileQuadtree.h"
#include "j1Window.h"
#include "j1App.h"

using namespace std;

class TileData
{

public:
	SDL_Texture * texture;
	int					x;
	int					y;
	int					scale;
	int					pivot_x;
	int					pivot_y;
	SDL_Rect*			section;
	Collider*			col;
	SDL_Rect			rect;
	float				speed;
	double				angle;
	uint				id;
	float				order;
	float				height;
	float				blitScale;
	bool				flip;
	bool				behind = false;
	iPoint				margin;

public:
	TileData() :
		id(0), x(0), y(0), order(0), height(0), scale(App->win->GetScale()), pivot_x(INT_MAX), pivot_y(INT_MAX),
		angle(0), speed(1), texture(nullptr), section(nullptr), rect({ 0,0,0,0 }), flip(false), col(nullptr), blitScale(1.0f), margin({ 0,0 }) {}
	TileData(uint id, int x, int y, float order, float height) :
		id(id), x(x), y(y), order(order), height(height), scale(App->win->GetScale()), pivot_x(INT_MAX), pivot_y(INT_MAX),
		angle(0), speed(1), texture(nullptr), section(nullptr), rect({ 0,0,0,0 }), flip(false), col(nullptr), blitScale(1.0f), margin({ 0,0 }) {}
	TileData(uint id, int x, int y, float order, float height, SDL_Texture* texture, SDL_Rect* section, bool flip, Collider* col, float scale) :
		id(id), x(x), y(y), order(order), height(height), scale(App->win->GetScale()), pivot_x(INT_MAX), pivot_y(INT_MAX),
		angle(0), speed(1), texture(texture), section(section), rect({ 0,0,0,0 }), flip(flip), col(col), blitScale(scale), margin({ 0,0 }) {}

	float Ordering()const
	{
		return order;
	}
};

struct Comparer
{
	bool operator()(const TileData* img1, const TileData* img2)const
	{
		return img1->Ordering() > img2->Ordering();
	}
};

struct TileComparer
{
	bool operator()(const TileData* img1, const TileData* img2)const
	{
		return img1->y > img2->y || (img1->y == img2->y && img1->x > img2->x);
	}
};

class j1Render : public j1Module
{
public:

	j1Render();

	// Destructor
	virtual ~j1Render();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	// Blit
	void SetViewPort(const SDL_Rect& rect);
	void ResetViewPort();
	iPoint ScreenToWorld(int x, int y) const;

	// Draw & Blit
	bool OrderBlit(priority_queue<TileData*, vector<TileData*>, Comparer>& priority)const;
	bool BlitHUD(SDL_Texture* texture, int x, int y, const SDL_Rect* section = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, float speed = 1.0f, float blitScale = 1.0f, double angle = 0, int pivot_x = INT_MAX, int pivot_y = INT_MAX, bool use_camera = true) const;
	bool Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, bool use_camera = true, float blitscale = 1.0f, float scale = App->win->GetScale(), SDL_Renderer* renderer_ = App->render->renderer,  float speed = 1.0f, double angle = 0, int pivot_x = INT_MAX, int pivot_y = INT_MAX) const;
	bool BlitMinimap(SDL_Texture* texture, int x, int y, const SDL_Rect* section = NULL, bool use_camera = true, SDL_Renderer* renderer_ = App->render->renderer, float scale = App->win->GetScale(), float speed = 1.0f, double angle = 0, int pivot_x = INT_MAX, int pivot_y = INT_MAX) const;
	bool BlitDialog(SDL_Texture* texture, int x, int y, const SDL_Rect* section = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, float speed = 1.0f, float Scale = 1, double angle = 0, int pivot_x = INT_MAX, int pivot_y = INT_MAX) const;
	bool DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool filled = true, bool use_camera = true) const;
	bool DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true) const;
	bool DrawCircle(int x1, int y1, int redius, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true) const;

	void reOrder();

	// Set background color
	void SetBackgroundColor(SDL_Color color);

public:

	int initialCameraX;
	int initialCameraY;


	priority_queue <TileData*, vector<TileData*>, Comparer> OrderToRender;
	SDL_Renderer*	renderer = nullptr;
	SDL_Rect		camera;
	SDL_Rect		viewport;
	SDL_Color		background;

	priority_queue <TileData*, vector<TileData*>, Comparer> map_sprites_priority;
	vector<TileData*> map_sprites;
	vector<TileData*> entities_sprites;

	bool			usingVsync = false;
};

#endif // __j1RENDER_H__