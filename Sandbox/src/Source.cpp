#include "iw/engine/EntryPoint.h"
#include "iw/engine/ImGui/ImGuiLayer.h"
#include "iw/engine/Entity/EntityLayer.h"
#include "iw/entity/Space.h"

struct Position {
	float x, y, z;
};

class Game : public IwEngine::Application {
public:
	Game() {
		IwEntity2::Space space;

		IwEntity2::Entity entity = space.CreateEntity();
		space.CreateComponent<Position>(entity);

		PushLayer(new IwEngine::EntityLayer());
	}
};

IwEngine::Application* CreateApplication() {
	return new Game();
}
