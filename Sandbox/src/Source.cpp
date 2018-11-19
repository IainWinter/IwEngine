#include <iostream>

#include "IwEngine/IwEngine.h"
#include "IwEcs/ecs_manager.h"

#include <vector>
#include <iostream>


class intfloat_system {
public:
	void update() {
		std::vector<int> ints;

		ints.push_back(1);
		ints.push_back(2);
		ints.push_back(3);

		std::vector<int>::iterator itr = ints.begin();
		std::vector<int>::iterator end = ints.end();

		for (; itr != end; itr++) {
			std::cout << *itr << std::endl;
		}
	}
};

class Game : public IwEngine::Application {
public:
	Game() {}
	~Game() {}

	void Run() override {
		iwecs::ecs_manager m = iwecs::ecs_manager();

		for (int i = 0; i < 100; i++) {
			m.create_entity<int, float>(1, 2.0f);
		}
		
		intfloat_system ifs = intfloat_system();
		ifs.update();
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