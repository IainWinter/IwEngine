#include "EntryPoint.h"

#include <gl/glew.h>
#include <gl/wglew.h>

class Game : public IwEngine::Application {
public:
	void Start() override {
		glClearColor(0.5, 0.3, 0.87, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		window.Render();

		space.Make_Subspace("Physics");
		space.Make_Subspace("Renderer");

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
	game->Initilize(options);

	return game;
}
