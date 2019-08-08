#include "iw/engine/EntryPoint.h"
#include "Layers/GameLayer.h"

#include "imgui/imgui.h"

#include "iw/entity2/Space.h"
#include "iw/util/memory/pool_allocator.h"

class Game
	: public IwEngine::Application
{
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

		return 0;
	}
};

struct int3 {
	int x, y, z;
};

IwEngine::Application* CreateApplication(
	IwEngine::InitOptions& options)
{
	IwEntity2::Space space;
	IwEntity2::Entity e = space.CreateEntity();
	space.CreateComponent<int3>(e);

	options.WindowOptions = IwEngine::WindowOptions {
		1280,
		720,
		true,
		IwEngine::NORMAL,
	};

	return new Game();
}
