#include "iw/engine/EntryPoint.h"
#include "iw/engine/ImGui/ImGuiLayer.h"
#include "iw/engine/Entity/EntityLayer.h"

class Game : public IwEngine::Application {
public:
	Game() {
		PushOverlay(new IwEngine::ImGuiLayer());
		PushLayer(new IwEngine::EntityLayer());
	}
};

IwEngine::Application* CreateApplication() {
	return new Game();
}
