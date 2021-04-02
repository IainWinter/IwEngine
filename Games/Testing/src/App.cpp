#include "App.h"
#include "Events/ActionEvents.h"
#include "iw/physics/Plane.h"
#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/audio/AudioSpaceStudio.h"

#include "Layers/SandboxLayer.h"

#include "iw/util/io/File.h"

#include "iw/engine/Layers/ImGuiLayer.h" // This shouldnt be here

#include <stack>

App::App() : iw::Application() {
	PushLayer<iw::SandboxLayer>();
}

int App::Initialize(
	iw::InitOptions& options)
{
	int err = Application::Initialize(options);

	if (!err) {
		iw::ref<iw::Context> context = Input->CreateContext("Game");
		
		context->MapButton(iw::SPACE, "+jump");
		context->MapButton(iw::SHIFT, "-jump");
		context->MapButton(iw::D    , "+right");
		context->MapButton(iw::A    , "-right");
		context->MapButton(iw::W    , "+forward");
		context->MapButton(iw::S    , "-forward");
		context->MapButton(iw::C    , "use");
		context->MapButton(iw::T    , "toolbox");
		context->MapButton(iw::I    , "imgui");
		context->MapButton(iw::R    , "reload");

		context->AddDevice(Input->CreateDevice<iw::Mouse>());
		context->AddDevice(Input->CreateDevice<iw::RawKeyboard>());
		context->AddDevice(Input->CreateDevice<iw::RawMouse>());
	}

	iw::ImGuiLayer* imgui = GetLayer<iw::ImGuiLayer>("ImGui"); // need this because layer is created in a different dll

	if (imgui) {
		imgui->BindContext();
	}

	return err;
}

bool App::HandleCommand(
	const iw::Command& command)
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

	else if (command.Verb == "use") {
		Bus->push<UseEvent>();
	}

	return Application::HandleCommand(command);
}

iw::Application* CreateApplication(
	iw::InitOptions& options)
{
	options.WindowOptions = iw::WindowOptions {
		1920/**4/3*/,
		1080/**4/3*/,
		false,
		iw::DisplayState::NORMAL
	};

	return new App();
}

iw::Application* GetApplicationForEditor() {
	return new App();
}
