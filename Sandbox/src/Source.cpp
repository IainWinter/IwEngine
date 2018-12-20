#include <iostream>
#include "Core/EntryPoint.h"
#include "ecs_manager.h"
#include "tight_array.h"
#include "system_manager.h"

struct Position {
	float x, y, z;
	float rotation;
};

struct Velocity {
	float x, y, z;
};

struct Mesh {
	int vert_count;
};

struct Collider {
	int vert_count;
};

struct Player {
	int speed;
};

struct Asteriod {
	int level;
};

struct Bullet {
	float spawn_time;
	int speed;
};

class MovementSystem 
	: iwecs::system<Position, Velocity>
{
	void update(
		view<Position, Velocity> view)
	{
		for (auto& c : view) {
			
		}
	}
};


class Game : public IwEngine::Application {
public:
	Game() {}
	~Game() {}

	void Run() override {
		iwecs::registry registry = iwecs::registry();

		//Player
		auto player = registry.create();
		registry.assign<Position>(player);
		registry.assign<Velocity>(player);
		registry.assign<Mesh>(player);
		registry.assign<Collider>(player);
		registry.assign<Player>(player);
		std::min


		//Physics system
		registry.view<Position, Velocity, Collider>();

		//Player system
		registry.view<Collider, Velocity, Player>();

		//Asteroid
		registry.view<Collider, Asteriod>();

		//Bullet
		registry.view<Collider, Bullet>();

	//	iwecs::ecs_manager m = iwecs::ecs_manager();
	//	
	//	for (int i = 0; i < 500; i++) {
	//		float f = i + .1f;
	//		iwecs::entity_t e = m.create_entity<int, float>(i, f);
	//	}

	//	m.destroy_entity(5);
	//	m.destroy_entity(85);
	//	m.destroy_entity(320);
	//	m.destroy_entity(199);

	//	for (int i = 0; i < 60; i++) {
	//		float f = i + .1f;
	//		iwecs::entity_t e = m.create_entity<int, float>(i, f);
	//	}

	//	m.create_entity<int, float>(2, 2.3f);
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