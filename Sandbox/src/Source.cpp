#include <iostream>

#include "IwMath/vector2.h"
#include "IwEngine/IwEngine.h"

#include "IwEcs/component_registry.h"
#include <tuple>

class Game : public IwEngine::Application {
public:
	Game() {}
	~Game() {}

	void Run() override {
		iwecs::component_registry c = iwecs::component_registry();
		c.add_archtype<int, float>();
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