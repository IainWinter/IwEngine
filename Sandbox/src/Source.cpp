#include <iostream>

#include "IwEngine/IwEngine.h"
#include "IwEcs/ecs_manager.h"

class Game : public IwEngine::Application {
public:
	Game() {}
	~Game() {}

	void Run() override {
		iwecs::component_registry c = iwecs::component_registry();
		c.create_entity<int, float>();
	}
};

IwEngine::Application* CreateApplication() {
	return new Game();
}


//iwm	iwmath
//iwecs	iwecs
//iwevt	iwevents
//iwi	iwinput
//iwe	iwengine
//iwp	iwphysics
//iwa	iwaudio