#include "Core/EntryPoint.h"
#include "set/sparse_set.h"

#include <iostream>

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
		iwu::sparse_set<int, Position> set = iwu::sparse_set<int, Position>();
		set.insert(3);
		set.insert(1);
		set.insert(0);
		set.erase(1);

		auto itr = set.begin();

		//iwecs::component_map world = iwecs::component_map();

		//world.emplace<Position>(0, Position {1, 2});
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