#include "iw/engine/EntryPoint.h"
#include "GameLayer.h"

class Game 
	: public IwEngine::Application
{
public:
	Game() {
		PushLayer(new GameLayer());
	}
};

IwEngine::Application* CreateApplication() {
	return new Game();
}
