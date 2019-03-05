#include "EntryPoint.h"

class Game : public IwEngine::Application {
public:

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
