#include "Core/EntryPoint.h"
#include "component/component_map.h"

struct Position {
	float x, y;
};

struct Velocity {
	float x, y;
};

class Game : public IwEngine::Application {
public:
	Game() {}
	~Game() {}

	void Run() override {
		iwecs::component_map world = iwecs::component_map();

		world.emplace<Position>(0, Position {1, 2});
		//world.erase<Position>(0);
		//world.emplace<Position, Velocity>(0, Position{ 1, 2 }, Velocity{3, 4});
		//world.erase(0);
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