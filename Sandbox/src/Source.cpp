#include "iw/engine/EntryPoint.h"
//#include "Layers/GameLayer.h"
#include "Layers/GameLayer3D.h"

#include "imgui/imgui.h"

#include "iw/entity/ComponentManager.h"
#include "iw/entity/Archetype.h"

#include "iw/engine/Time.h"
#include "iw/engine/Layers/DebugLayer.h"

#include "iw/events/eventbus.h"

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
	: public IW::Application
{
public:
	Game() {
		InputManager.CreateDevice<IW::Mouse>();
		//InputManager.CreateDevice<IW::RawMouse>();
		InputManager.CreateDevice<IW::RawKeyboard>();

		PushLayer<GameLayer3D>();
	}

	int Initialize(
		IW::InitOptions& options) override
	{
		Application::Initialize(options);

		ImGui::SetCurrentContext((ImGuiContext*)options.ImGuiContext);

		return 0;
	}
};

IW::Application* CreateApplication(
	IW::InitOptions& options)
{
	options.WindowOptions = IW::WindowOptions {
		1280,
		720,
		true,
		IW::DisplayState::NORMAL
	};

	return new Game();
}
