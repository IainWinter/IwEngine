#include <iostream>

#include "Core/EntryPoint.h"
#include "ecs_manager.h"

#include <vector>
#include <iostream>
#include <list>

class Game : public IwEngine::Application {
public:
	Game() {}
	~Game() {}

	void Run() override {
		iwecs::ecs_manager m = iwecs::ecs_manager();
		
		for (int i = 0; i < 500; i++) {
			float f = i + .1f;
			iwecs::entity_t e = m.create_entity<int, float>(i, f);
		}

		m.destroy_entity(5);
		m.destroy_entity(85);
		m.destroy_entity(320);
		m.destroy_entity(199);

		for (int i = 0; i < 60; i++) {
			float f = i + .1f;
			iwecs::entity_t e = m.create_entity<int, float>(i, f);
		}

		m.create_entity<int, float>(2, 2.3f);
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