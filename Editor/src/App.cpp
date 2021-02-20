#include "App.h"
#include "Events/ActionEvents.h"
#include "iw/physics/Plane.h"
#include "iw/physics/Collision/PlaneCollider.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/audio/AudioSpaceStudio.h"

#include "Layers/SimpleSandLayer.h"
#include "Layers/ToolLayer.h"

namespace iw {
	App::App() {
		Audio = REF<AudioSpaceStudio>("assets/sounds/");

		iw::ref<Context> context = Input->CreateContext("Editor");
		
		context->MapButton(iw::SPACE, "+jump");
		context->MapButton(iw::SHIFT, "-jump");
		context->MapButton(iw::D    , "+right");
		context->MapButton(iw::A    , "-right");
		context->MapButton(iw::W    , "+forward");
		context->MapButton(iw::S    , "-forward");
		context->MapButton(iw::C    , "use");
		context->MapButton(iw::T    , "toolbox");
		context->MapButton(iw::I    , "imgui");

		iw::ref<Device> m  = Input->CreateDevice<Mouse>();
		//iw::ref<Device> rm = Input->CreateDevice<RawMouse>();
		iw::ref<Device> k  = Input->CreateDevice<RawKeyboard>();
		
		context->AddDevice(m);
		//context->AddDevice(rm);
		context->AddDevice(k);

		sandbox = PushLayer<SimpleSandLayer>(); // model system breaks in test layer
		imgui   = PushLayer<ImGuiLayer>(/*Window()*/);
	}

	int App::Initialize(
		iw::InitOptions& options)
	{
		int err = Application::Initialize(options);

		if (!err) {
			ImGuiLayer* imgui = GetLayer<ImGuiLayer>("ImGui");
			if (imgui) {
				imgui->BindContext();
			}
		}

		toolbox = PushLayer<ToolLayer>(sandbox->GetMainScene());
		err = toolbox->Initialize();

		PopLayer(toolbox);
		PopLayer(imgui);

		return err;
	}

	void App::Update() {
		if (GetLayer("Toolbox") != nullptr) {
			//sandbox->Update();
			//sandbox->FixedUpdate();
			sandbox->UpdateSystems();
		}

		Application::Update();
	}

	void Editor::App::FixedUpdate() {
		//if (GetLayer("Toolbox") == nullptr) {
			Application::FixedUpdate();
		//}
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

		else if (command.Verb == "use") {
			Bus->push<UseEvent>();
		}

		else if (command.Verb == "toolbox") {
			bool dev = GetLayer("Toolbox") != nullptr;
			if (dev) {
				PopLayer(toolbox);
				PushLayer(sandbox);
			}

			else {
				PushLayer(toolbox);
				PopLayer(sandbox);
			}

			Bus->send<DevConsoleEvent>(dev);
		}

		else if (command.Verb == "imgui") {
			if (GetLayer("ImGui") == nullptr) {
				PushLayer(imgui);
				Bus->push<WindowResizedEvent>(Window()->Id(), Renderer->Width(), Renderer->Height());
			}

			else {
				PopLayer(imgui);
			}
		}

		return Application::HandleCommand(command);
	}
}

iw::Application* CreateApplication(
	iw::InitOptions& options)
{
	options.WindowOptions = iw::WindowOptions {
		1920/**4/3*/,
		1080/**4/3*/,
		true,
		iw::DisplayState::NORMAL
	};
	
	return new iw::App();
}
