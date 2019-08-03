#include "iw/engine/EntryPoint.h"
#include "GameLayer.h"
#include "BulletSystem.h"

#include "iw/util/queue/blocking_queue.h"

#include <thread>
#include <vector>

#include "imgui/imgui.h"

class Game
	: public IwEngine::Application
{
public:
	Game() {
		InputManager.CreateDevice<IwInput::Mouse>();
		//InputManager.CreateDevice<IwInput::RawKeyboard>();

		PushLayer<GameLayer>();
		PushSystem<BulletSystem>();
	}

	int Initialize(
		IwEngine::InitOptions& options) override
	{
		Application::Initialize(options);

		ImGui::SetCurrentContext((ImGuiContext*)options.ImGuiContext);

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
		IwEngine::NORMAL,
	};

	return new Game();
}
