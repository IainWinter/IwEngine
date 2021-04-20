#include "iw/engine/EntryPoint.h"
#include "iw/engine/Layers/ImGuiLayer.h"
#include "Layers/TestLayer.h"

class App : public iw::Application {
public:
	App() : iw::Application() {
		PushLayer<iw::TestLayer>();
	}

	int Initialize(
		iw::InitOptions& options)
	{
		int err = Application::Initialize(options);

		if (!err) {
			iw::ref<iw::Context> context = Input->CreateContext("Game");

			context->MapButton(iw::T, "toolbox");
			context->MapButton(iw::I, "imgui");
			context->MapButton(iw::R, "reload");

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
};

iw::Application* CreateApplication(
	iw::InitOptions& options)
{
	options.WindowOptions = iw::WindowOptions {
		1920/**4/3*/,
		1080/**4/3*/,
		true,
		iw::DisplayState::NORMAL
	};

	return new App();
}

iw::Application* GetApplicationForEditor() {
	return new App();
}
