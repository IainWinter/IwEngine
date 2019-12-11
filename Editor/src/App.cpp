#include "App.h"

#include "Layers/ToolLayer.h"
#include "Layers/SandboxLayer.h"
#include "Events/ActionEvents.h"

#include "iw/physics/Dynamics/DynamicsSpace.h"

namespace IW {
	App::App() {
		iw::ref<Context> context = Input->CreateContext("Sandbox");
		
		//context = Input->GetContex("Sandbox");

		//Input->MapButton("Sandbox", IW::SPACE, "+jump");
		//Input->MapButton("Sandbox", IW::SPACE, "+jump");

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

		Rigidbody* object = new Rigidbody();
		object->SetTakesGravity(true);
		object->SetSimGravity(true);

		DynamicsSpace space;
		space.SetGravity(iw::vector3(0, -9.8f, 0));
		space.AddRigidbody(object);
		space.Step(1 / 60.0f);

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
