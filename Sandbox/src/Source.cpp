#include "iw/engine/EntryPoint.h"
#include "iw/engine/Entity/EntityLayer.h"

class Game : public IwEngine::Application {
public:
	Game() {
		PushLayer(new IwEngine::EntityLayer());
	}
};

IwEngine::Application* CreateApplication() {
	return new Game();
}
