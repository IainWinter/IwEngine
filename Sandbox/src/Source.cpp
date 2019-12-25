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
		iw::ref<IW::Context> context = Input->CreateContext("Sandbox");

		context->MapButton(IW::SPACE, "+jump");
		context->MapButton(IW::SHIFT, "-jump");
		context->MapButton(IW::RIGHT, "+right");
		context->MapButton(IW::LEFT,  "-right");
		context->MapButton(IW::UP,    "+forward");
		context->MapButton(IW::DOWN,  "-forward");
		context->MapButton(IW::X,     "dssh");

		iw::ref<IW::Device> m  = Input->CreateDevice<IW::Mouse>();
		iw::ref<IW::Device> rm = Input->CreateDevice<IW::RawMouse>();
		iw::ref<IW::Device> k  = Input->CreateDevice<IW::RawKeyboard>();

		context->AddDevice(m);
		context->AddDevice(rm);
		context->AddDevice(k);

		PushLayer<GameLayer3D>();
	}

	int Initialize(
		IW::InitOptions& options) override
	{
		Application::Initialize(options);
		GetLayer<IW::ImGuiLayer>("ImGui")->BindContext();

		return 0;
	}
};

IW::Application* CreateApplication(
	IW::InitOptions& options)
{
	options.WindowOptions = {
		1280,
		720,
		true,
		IW::DisplayState::NORMAL
	};

	return new Game();
}
