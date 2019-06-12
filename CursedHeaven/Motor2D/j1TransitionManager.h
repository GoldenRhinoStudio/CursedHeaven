#ifndef _j1TRANSITIONMANAGER_H__
#define _j1TRANSITIONMANAGER_H__

#include "j1Module.h"
#include "j1Color.h"
#include "j1Transitions.h"
#include "j1SceneMenu.h"
#include <list>

class j1TransitionManager : public j1Module {
public:

	j1TransitionManager();
	~j1TransitionManager();

	virtual bool Awake(pugi::xml_node&) { return true; }
	virtual bool Start() { return true; }
	virtual bool PostUpdate();
	virtual bool CleanUp();

	// Save and Load
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	void FadingToColor(SCENE scene_to_remove, SCENE scene_to_appear, j1Color color = Black, float time = 1.0f);
	void Wiping(SCENE scene_to_remove, SCENE scene_to_appear, j1Color color = Black, float time = 0.5f);
	void LinesAppearing(SCENE scene_to_remove, SCENE scene_to_appear, j1Color color = Black, float time = 2.0f);
	void SquaresAppearing(SCENE scene_to_remove, SCENE scene_to_appear, int transition = 1, j1Color color = Black, float time = 2.0f);

	void CleanTransitions(j1Transitions* ended_transition);

	void SwitchScenes(SCENE scene1, SCENE scene2);

public:
	bool transition;

	bool scene1active = false;
	bool scene2active = false;

private:
	std::list<j1Transitions*> transitions_list;
	SDL_Rect screen;

};

#endif