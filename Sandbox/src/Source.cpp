#include "EntryPoint.h"
#include "log.h"

#include <iostream>

class Game : public IwEngine::Application {
public:
	Game() {}

	void Start() override {
		LOG_INFO("This is a test");

		IwEngine::Application::Start();
	}
};

IwEngine::Application* CreateApplication() {
	return new Game();
}
