#include "iw/engine/EntryPoint.h"
#include "Layers/GameLayer.h"

#include "iw/util/queue/blocking_queue.h"

#include <thread>
#include <vector>

#include "imgui/imgui.h"

#include "iw/util/memory/linear_allocator.h"

#include "iw/util/async/potential.h"

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

		iwu::potential<int*> pi;

		std::thread t([](iwu::potential<int*> p) {
			Sleep(2000);
			p.initialize(new int(5));
		}, pi);

		while(!pi.initialized()) {}

		LOG_INFO << pi.value();

		t.join();

		delete pi.value();
		pi.release();

		return 0;
	}

	void Run() override {
		while (true) {}
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
