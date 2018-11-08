#include <iostream>

#include "IwMath/vector2.h"
#include "IwEngine/IwEngine.h"

#include "IwEcs/memory_chunk.h"

class Game : public IwEngine::Application {
public:
	Game() {}
	~Game() {}

	void Run() override {
		memory_chunk<int> c = memory_chunk<int>(64);
		while (c.alloc(5)) {};
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