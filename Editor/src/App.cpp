#include "App.h"
#include "imgui/imgui.h"

#include "Layers/ToolLayer.h"
#include "Layers/SandboxLayer.h"

namespace IW {
	App::App() {
		InputManager->CreateDevice<Mouse>();
		InputManager->CreateDevice<RawKeyboard>();

		PushOverlay<ImGuiLayer>();
		PushLayer<ToolLayer>();
		PushLayer<SandboxLayer>();
	}

	int App::Initialize(
		IW::InitOptions& options)
	{
		Application::Initialize(options);

		ImGui::SetCurrentContext((ImGuiContext*)options.ImGuiContext);

		return 0;
	}
}

IW::Application* CreateApplication(
	IW::InitOptions& options)
{
	options.WindowOptions = IW::WindowOptions{
		1280,
		720,
		true,
		IW::DisplayState::NORMAL
	};

	return new IW::App();
}
