#include "iw/engine/EntryPoint.h"
#include "Layers/GameLayer.h"

#include "imgui/imgui.h"

//#include "iw/entity2/Space.h"

#include "iw/entity/ComponentManager.h"

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
		
		/*
		IwEntity::Space space;
		
		// 1

		Entity e = space.CreateEntity<Pos, Vel>();

		// 2

		Archetype a = space.CreateArchetype<Pos, Vel>();
		Entity e = space.CreateEntity(a);
		

		

		IwEntity::EntityArchetype archetype = space.CreateArchetype<Position, Velocity>();

		IwEntity::Enttiy entity = space.CreateEntity(archetype);



		IwEntity::
			*/
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
