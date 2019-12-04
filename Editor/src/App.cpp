#include "App.h"

#include "Layers/ToolLayer.h"
#include "Layers/SandboxLayer.h"

namespace IW {
	App::App() {
		//iw::ref<Context>& context = Input->CreateContext("Sandbox");
		//
		//context = Input->GetContex("Sandbox");

		//Input->MapButton("Sandbox", IW::SPACE, "+jump");
		//Input->MapButton("Sandbox", IW::SPACE, "+jump");

		//context.MapButton(IW::SPACE, "+jump");

		//auto& m  = Input->CreateDevice<Mouse>();
		//auto& rm = Input->CreateDevice<RawMouse>();
		//auto& k  = Input->CreateDevice<RawKeyboard>();

		//c->AddDevice(m);
		//c->AddDevice(rm);
		//c->AddDevice(k);

		PushLayer<ToolLayer>();
		PushLayer<SandboxLayer>();

		std::string s = "resize_window 100 100";

		Console->HandleCommand(s);
	}

	int App::Initialize(
		IW::InitOptions& options)
	{
		int err = Application::Initialize(options);

		if (!err) {
			GetLayer<ImGuiLayer>("ImGui")->BindContext();
		}

		return err;
	}
}

IW::Application* CreateApplication(
	IW::InitOptions& options)
{
	options.WindowOptions = IW::WindowOptions {
		1280,
		720,
		true,
		IW::DisplayState::NORMAL
	};

	return new IW::App();
}
