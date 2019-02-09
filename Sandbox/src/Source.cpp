#include "Core/EntryPoint.h"
#include "component/bag.h"

#include "type/type_group.h"

#include <iostream>

struct Position {
	int x, y;
};

struct Velocity {
	int x, y;
};

class Game : public IwEngine::Application {
public:
	Game() {}
	~Game() {}

	void Run() override {
		iwecs::bag<size_t> world = iwecs::bag<size_t>();

		world.emplace<Position>(0, Position{ 1, 2 });
		world.emplace<Velocity>(0, Velocity{ 1, 2 });

		world.emplace<Position>(1, Position{ 1, 2 });
		
		world.emplace<Velocity>(2, Velocity{ 1, 2 });
		world.emplace<Position>(2, Position{ 1, 2 });

		auto view = world.view<Position, Velocity>();

		std::cin.get();
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