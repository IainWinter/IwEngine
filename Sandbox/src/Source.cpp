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
		iwecs::component_registry c = iwecs::component_registry();

		c.create_entity<int, float>(1, 2.0f);
		c.create_entity<int, float>(3, 4.0f);
		c.create_entity<double, float>(5, 6.0f);

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