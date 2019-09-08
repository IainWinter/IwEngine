#include "iw/engine/EntryPoint.h"
#include "Layers/GameLayer.h"

#include "imgui/imgui.h"

//#include "iw/entity2/Space.h"

#include "iw/entity/ComponentManager.h"
#include "iw/entity/Archetype.h"

#include "iw/engine/Time.h"

struct Position {
	float x, y, z;
};

struct Velocity {
	float x, y, z;
};

class Game
	: public IwEngine::Application
{
private:
	//IwEntity2::Space space;

public:
	Game() {
		InputManager.CreateDevice<IwInput::Mouse>();
		//InputManager.CreateDevice<IwInput::RawKeyboard>();

		PushLayer<GameLayer>();
	}

	int Initialize(
		IwEngine::InitOptions& options) override
	{
		Application::Initialize(options);

		ImGui::SetCurrentContext((ImGuiContext*)options.ImGuiContext);

		IwEntity::Space space;

		auto p  = space.RegisterComponent<Position>();
		auto v  = space.RegisterComponent<Velocity>();
		auto p2 = space.RegisterComponent(typeid(Position), sizeof(Position));
		auto v2 = space.RegisterComponent(typeid(Velocity), sizeof(Velocity));

		IwEntity::EntityArchetype a  = space.CreateArchetype({ p, v });
		IwEntity::EntityArchetype a1 = space.CreateArchetype<Position, Velocity>();

		IwEntity::Entity2 e  = space.CreateEntity(a);
		IwEntity::Entity2 e1 = space.CreateEntity<Position, Velocity>();

		//IwEntitiy::View view = space.QueryEntities<Position, Velocity>();

		//struct Components {
		//	Position& Position;
		//	Velocity& Velocity;
		//};

		//for (IwEntity::Entity entity : view) {
		//	Components& c = entity.Components.Tie<Components>();
		//	c.Position += c.Velocity;
		//}

		return 0;
	}
};

IwEngine::Application* CreateApplication(
	IwEngine::InitOptions& options)
{
	options.WindowOptions = IwEngine::WindowOptions {
		1280,
		720,
		true,
		IwEngine::NORMAL
	};

	return new Game();
}
