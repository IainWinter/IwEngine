#include "EntryPoint.h"

#include <gl/glew.h>
#include <gl/wglew.h>

class TestLayer : public IwEngine::Layer {
public:
	TestLayer()
		: IwEngine::Layer("Test") 
	{}

	int Initilize() override {
		LOG_DEBUG << "Test Layer initilizing";
		return 0;
	}

	void Start() override {
		LOG_DEBUG << "Test Layer started";
	}

	void Stop() override {
		LOG_DEBUG << "Test Layer stopped";
	}

	void Destroy() override {
		LOG_DEBUG << "Test Layer destroyed";
	}

	void Update() override {
		LOG_INFO << "Test Layer updated";
	}

	void OnEvent(IwEngine::Event& e) override {
		LOG_DEBUG << "Test Layer received event " << e.type;
	}
};

class Game : public IwEngine::Application {
public:
	int Initilize(
		IwEngine::WindowOptions& options) override
	{
		PushLayer(new TestLayer());

		return Application::Initilize(options);
	}

	void Start() override {
		glClearColor(0.5, 0.3, 0.87, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		window.Render();


		IwEngine::Application::Start();
	}
};

IwEngine::Application* CreateApplication() {
	IwEngine::WindowOptions options {
		1280,
		720,
		IwEngine::NORMAL,
		true,
	};

	Game* game = new Game();
	game->Initilize(options); //Not where it should be

	return game;
}


