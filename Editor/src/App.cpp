#include "App.h"

#include "Layers/ToolLayer.h"
#include "Layers/SandboxLayer.h"
#include "Events/ActionEvents.h"

namespace IW {
	App::App() {
		iw::ref<Context> context = Input->CreateContext("Sandbox");
		
		context->MapButton(IW::SPACE, "+jump");
		context->MapButton(IW::SHIFT, "-jump");
		context->MapButton(IW::D    , "+right");
		context->MapButton(IW::A    , "-right");
		context->MapButton(IW::W    , "+forward");
		context->MapButton(IW::S    , "-forward");
		context->MapButton(IW::E    , "use");

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

		return err;
	}

	bool App::HandleCommand(
		const Command& command)
	{
		if (command.Verb == "jump") {
			Bus->push<JumpEvent>(command.Active);
		}

		else if (command.Verb == "right") {
			Bus->push<RightEvent>(command.Active);
		}

		else if (command.Verb == "forward") {
			Bus->push<ForwardEvent>(command.Active);
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
