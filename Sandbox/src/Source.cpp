#include <iostream>

#include "IwMath/vector2.h"
#include "IwEngine/IwEngine.h"

class Game : public IwEngine::Application {
public:
	Game() {}
	~Game() {}

	void Run() override {
		iwm::vector2 xy = iwm::vector2();
		iwm::vector2 v  = iwm::vector2(1, 2);

		bool running = true;
		while (running) {
			std::cout << xy << std::endl;
			xy += v;
			if (std::cin.get() == 'A') {
				running = false;
			}
		}
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