#include <iostream>
#include "EntryPoint.h"
#include "logger.h"

#include "input_manager.h"

class Game : public IwEngine::Application {
public:
	Game() {}

	void OnEvent(IwEngine::Event& e) override {
		IwEngine::Application::OnEvent(e);
	}
};

IwEngine::Application* CreateApplication() {
	return new Game();
}
