#include <iostream>
#include "EntryPoint.h"

#include "logger.h"

class Game : public IwEngine::Application {
public:
	Game() {}

	void Initilize() {
		IwEngine::Application::Initilize();
	}

	void Start() {
		IwEngine::Application::Start();
	}

	void Stop() {
		IwEngine::Application::Stop();
	}

	void Destroy() {
		IwEngine::Application::Destroy();
	}

	void Run() override {
		window.Open();

		MiniLog::current_level() = logINFO;
		MINILOG(logDEBUG) << "DEBUG Log message";
		MINILOG(logINFO) << "INFO Log message";

		std::cin.get();
	}
};

IwEngine::Application* CreateApplication() {
	return new Game();
}
