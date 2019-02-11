#include "Core/EntryPoint.h"
#include "iwecs.h"

#include <iostream>

struct Position { int x, y;  };
struct Velocity { int x, y;  };
struct Mesh     { int verts; };
struct Collider { int verts; };
struct Player   {};
struct Bullet   {};
struct Asteroid { int level; };
struct Score    { int score; };

// components
// pos
// vel
// mesh
// col
// player
// asteroid
// score
// bullet
//
// groups
// pos, vel, col
// pos, mesh
//
// views
// vel, player
// pos, asteroid
// score
// bullet
//
// systems
// u pos, vel, col
// u pos, mesh
// u vel, player
// r pos, asteroid
// r score
// r bullet
//
// entities
// Player:     pos vel mesh col player
// Asteroid 1: pos vel mesh col        asteroid
// Asteroid 2: pos vel mesh col        asteroid
// Asteroid 3: pos vel mesh col        asteroid
// Asteroid 4: pos vel mesh col        asteroid
// Bullet 1:   pos vel mesh col                 bullet
// Bullet 2:   pos vel mesh col                 bullet
// Bullet 3:   pos vel mesh col                 bullet
// Score:      pos     mesh                            score

//archetypes
// group 1: pos vel mesh col player
// group 2: pos vel mesh col asteroid
// group 3: pos vel mesh col bullet
// group 4: pos mesh score

//data layout
//
// group 1
// Player: pos vel mesh col player
//
// group 2
// Asteroid 1: pos vel mesh col asteroid
// Asteroid 2: pos vel mesh col asteroid
// Asteroid 3: pos vel mesh col asteroid
// Asteroid 4: pos vel mesh col asteroid
//
// group 3
// Bullet 1: pos vel mesh col bullet
// Bullet 2: pos vel mesh col bullet
// Bullet 3: pos vel mesh col bullet
//
// group 4:
// Score: pos mesh score
//
//
//
//
// 
//

class Game : public IwEngine::Application {
public:
	void Run() override {
		iwecs::reg reg;
		auto e = reg.create();
		reg.assign<Position>(e, 1, 2);
		reg.assign<Velocity>(e, 1, 2);

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