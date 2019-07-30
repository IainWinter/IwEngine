#include "iw/engine/EntryPoint.h"
#include "GameLayer.h"

#include "iw/util/queue/blocking_queue.h"

#include <thread>
#include <vector>

#include "imgui/imgui.h"

struct Transform { float x, y, z; };
struct Velocity  { float vx, vy, vz; };
struct Collider  { float count; };

class Game 
	: public IwEngine::Application
{
public:
	Game() {
		InputManager.CreateDevice<IwInput::Mouse>();
		//InputManager.CreateDevice<IwInput::RawKeyboard>();

		PushLayer(new GameLayer());
	}

	int Initialize(
		const IwEngine::WindowOptions& windowOptions) override
	{
		Application::Initialize(windowOptions);

		ImGui::SetCurrentContext((ImGuiContext*)GetImGuiContext());

		return 0;
	}
};

IwEngine::Application* CreateApplication() {
	return new Game();
}
