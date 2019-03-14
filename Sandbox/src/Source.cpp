#include "iw/engine/EntryPoint.h"
#include "iw/engine/ImGui/ImGuiLayer.h"

class Game : public IwEngine::Application {
public:
	Game() {
		PushOverlay(new IwEngine::ImGuiLayer());
	}
};

IwEngine::Application* CreateApplication() {
	return new Game();
}
