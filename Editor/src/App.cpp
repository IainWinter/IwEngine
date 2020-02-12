#include "App.h"

#include "Layers/ToolLayer.h"
#include "Layers/SandboxLayer.h"
#include "Events/ActionEvents.h"
#include "iw/physics/Plane.h"
#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/physics/Collision/SphereCollider.h"

namespace iw {
	App::App() {
		iw::ref<Context> context = Input->CreateContext("Editor");
		
		context->MapButton(iw::SPACE, "+jump");
		context->MapButton(iw::SHIFT, "-jump");
		context->MapButton(iw::D    , "+right");
		context->MapButton(iw::A    , "-right");
		context->MapButton(iw::W    , "+forward");
		context->MapButton(iw::S    , "-forward");
		context->MapButton(iw::E    , "use");

		iw::ref<Device> m  = Input->CreateDevice<Mouse>();
		iw::ref<Device> rm = Input->CreateDevice<RawMouse>();
		iw::ref<Device> k  = Input->CreateDevice<RawKeyboard>();
		
		context->AddDevice(m);
		context->AddDevice(rm);
		context->AddDevice(k);

		active = false;
		toolbox = PushLayer<ToolLayer>();
		PushLayer<SandboxLayer>();
	}

	int App::Initialize(
		iw::InitOptions& options)
	{
		int err = Application::Initialize(options);

		if (!err) {
			GetLayer<ImGuiLayer>("ImGui")->BindContext();
		}

		PopLayer(toolbox);

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

		else if (command.Verb == "/") {
			if (active) {
				PopLayer(toolbox);
			}

			else {
				PushLayer(toolbox);
			}

			active = !active;
		}

		return Application::HandleCommand(command);
	}
}

iw::Application* CreateApplication(
	iw::InitOptions& options)
{
	options.WindowOptions = iw::WindowOptions {
		1280,
		720,
		true,
		iw::DisplayState::NORMAL
	};

	return new iw::App();
}
