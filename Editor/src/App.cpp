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
	}

	int App::Initialize(
		IW::InitOptions& options)
	{
		int err = Application::Initialize(options);

		if (!err) {
			GetLayer<ImGuiLayer>("ImGui")->BindContext();
		}

		Console->QueueCommand("resize_window 100 100");
		Console->QueueCommand("test 100.4 hello");
		Console->QueueCommand("hello world 100");
		Console->QueueCommand("this is a test 100 100");
		Console->QueueCommand("lkjh 1sdf00 10f.00f0 0x500");
		Console->ExecuteQueue();

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

		else if (command.Verb == "hello") {
			LOG_INFO << "Hello" 
				<< " "<< command.Tokens[0].String 
				<< " "<< command.Tokens[1].Int;
		}

		else if (command.Verb == "this") {
			LOG_INFO << "This"
				<< " "<< command.Tokens[0].String
				<< " "<< command.Tokens[1].String
				<< " "<< command.Tokens[2].String
				<< " "<< command.Tokens[3].Int
				<< " "<< command.Tokens[4].Int;
		}

		else if (command.Verb == "lkjh") {
			LOG_INFO << "LKJH" 
				<< " "<< command.Tokens[0].String
				<< " "<< command.Tokens[1].String
				<< " "<< command.Tokens[2].Float;
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
