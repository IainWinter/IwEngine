#include "App.h"

#include "Layers/ToolLayer.h"
#include "Layers/SandboxLayer.h"

namespace IW {
	App::App() {
		iw::ref<Context> context = Input->CreateContext("Sandbox");
		
		//context = Input->GetContex("Sandbox");

		//Input->MapButton("Sandbox", IW::SPACE, "+jump");
		//Input->MapButton("Sandbox", IW::SPACE, "+jump");

		context->MapButton(IW::SPACE, "+jump");

		iw::ref<Device> m  = Input->CreateDevice<Mouse>();
		iw::ref<Device> rm = Input->CreateDevice<RawMouse>();
		iw::ref<Device> k  = Input->CreateDevice<RawKeyboard>();
		
		context->AddDevice(m);
		context->AddDevice(rm);
		context->AddDevice(k);

		PushLayer<ToolLayer>();
		PushLayer<SandboxLayer>();
	}

	int App::Initialize(
		IW::InitOptions& options)
	{
		int err = Application::Initialize(options);

		if (!err) {
			GetLayer<ImGuiLayer>("ImGui")->BindContext();
		}

		Console->ExecuteCommand("resize_window 100 100");
		Console->ExecuteCommand("test 100.4 hello");
		//Console->ExecuteQueue();

		return err;
	}

	bool App::HandleCommand(
		const Command& command)
	{
		if (command.Verb == "test") {
			LOG_INFO << "Test" 
				<< " " << command.Tokens[0].Float
				<< " " << command.Tokens[1].String;
		}

		return Application::HandleCommand(command);
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
