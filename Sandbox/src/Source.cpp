#include "iw/engine/EntryPoint.h"
#include "Layers/GameLayer.h"

#include "imgui/imgui.h"

#include "iw/entity/ComponentManager.h"
#include "iw/entity/Archetype.h"

#include "iw/engine/Time.h"

struct Position {
	int x, y, z;
};

struct Velocity {
	int x, y, z;
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

		iwu::ref<const IwEntity::Component> p  = space.RegisterComponent<Position>();
		iwu::ref<const IwEntity::Component> v  = space.RegisterComponent<Velocity>();
		iwu::ref<const IwEntity::Component> p2 = space.RegisterComponent(typeid(Position), sizeof(Position));
		iwu::ref<const IwEntity::Component> v2 = space.RegisterComponent(typeid(Velocity), sizeof(Velocity));

		iwu::ref<const IwEntity::Archetype2> a  = space.CreateArchetype({ p, v });
		iwu::ref<const IwEntity::Archetype2> a3 = space.CreateArchetype<Position, Velocity>();
		iwu::ref<const IwEntity::Archetype2> a4 = space.CreateArchetype<Velocity, Position>();
		iwu::ref<const IwEntity::Archetype2> a5 = space.CreateArchetype<Velocity, Velocity, Velocity>();

		struct Components {
			Position* Position;
			Velocity* Velocity;
		};

		IwEngine::Time::Update();

		for (size_t i = 0; i < 1000000; i++) {
			iwu::ref<IwEntity::Entity2> entity = space.CreateEntity<Position, Velocity>();
		}

		IwEntity::ComponentQuery q  = space.MakeQuery<Position, Velocity>();
		IwEntity::EntityComponentArray r = space.Query(q);

		//IwEntity::EntityQuery q1 = space.CreateQuery(
		//	{ typeid(Position), typeid(Velocity) }
		//);

		//IwEntity::EntityArray array = space.ExecuteQuery(query);

		//IwEntity::EntityComponentData data1 = space.ExecuteQuery<Position, Velocity>();

		IwEngine::Time::Update();

		LOG_INFO << IwEngine::Time::DeltaTime();

		/*Position* pos = (Position*)e.ComponentData->Components[0];
		pos->x = 5;

		Position* pos1 = (Position*)e1.ComponentData->Components[0];
		pos1->x = 6;*/

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
