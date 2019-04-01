#include "iw/engine/EntryPoint.h"
#include "iw/engine/Entity/EntityLayer.h"

#include "iw/engine/Time.h"

#include <chrono>
#include <iostream>

class Game : public IwEngine::Application {
public:
	Game() {
		PushLayer(new IwEngine::EntityLayer());
	}
};

IwEngine::Application* CreateApplication() {
	return new Game();
}
