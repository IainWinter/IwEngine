#include <iostream>

#include "IwMath/vector2.h"
#include "IwEngine/IwEngine.h"

#include "IwEcs/memory_chunk.h"
#include <tuple>

class Game : public IwEngine::Application {
public:
	Game() {}
	~Game() {}

	void Run() override {
		memory_chunk<int, float> c = memory_chunk<int, float>(64);
		c.alloc(5, 5.5f);
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