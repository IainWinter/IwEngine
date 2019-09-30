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

struct Mesh {
	int Count;
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

		struct PhysicalComponents {
			Position* Position;
			Velocity* Velocity;
		};

		IwEngine::Time::Update();

		std::vector<IwEntity::Entity> entities;
		for (size_t i = 0; i < 100000; i++) {
			entities.push_back(space.CreateEntity<Position, Velocity>());
		}

		IwEngine::Time::Update();
		LOG_INFO << "Create: " << IwEngine::Time::DeltaTime();

		IwEntity::EntityComponentArray eca1 = space.Query<Position, Velocity>();

		IwEngine::Time::Update();
		LOG_INFO << "Query: " << IwEngine::Time::DeltaTime();

		int i = 0;
		for (auto& components : eca1) {
			auto [pos, vel] = components.Tie<PhysicalComponents>();
			pos->x = 1;
			vel->x = 2;
			i++;
		}

		IwEngine::Time::Update();
		LOG_INFO << "Iterate: " << IwEngine::Time::DeltaTime();

		LOG_INFO << i;

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
