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

		std::weak_ptr<IwEntity::Component> p  = space.RegisterComponent<Position>();
		std::weak_ptr<IwEntity::Component> v  = space.RegisterComponent<Velocity>();
		std::weak_ptr<IwEntity::Component> p2 = space.RegisterComponent(typeid(Position), sizeof(Position));
		std::weak_ptr<IwEntity::Component> v2 = space.RegisterComponent(typeid(Velocity), sizeof(Velocity));

		std::weak_ptr<IwEntity::Archetype2> a  = space.CreateArchetype({ p, v });
		std::weak_ptr<IwEntity::Archetype2> a1 = space.CreateArchetype<Position, Velocity>();

		std::vector<std::weak_ptr<IwEntity::Entity2>> entities;

		for (size_t i = 0; i < 100000; i++) {
			entities.push_back(space.CreateEntity<Position, Velocity>());
		}

		for (size_t i = 0; i < 100000; i += rand() / RAND_MAX * 100) {
			space.DestroyEntity(entities[i]);
		}

		for (size_t i = 0; i < 10; i++) {
			entities.push_back(space.CreateEntity<Position, Velocity>());
		}

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
