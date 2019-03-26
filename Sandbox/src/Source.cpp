#include "iw/engine/EntryPoint.h"
#include "iw/engine/ImGui/ImGuiLayer.h"
#include "iw/engine/Entity/EntityLayer.h"
#include "iw/entity/Space.h"

class Game : public IwEngine::Application {
public:
	Game() {
		PushLayer(new IwEngine::EntityLayer());
	}
};

IwEngine::Application* CreateApplication() {
	return new Game();
}
