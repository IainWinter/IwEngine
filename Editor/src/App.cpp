#include "App.h"

#include "Layers/ToolLayer.h"
#include "Layers/SandboxLayer.h"

#include "iw/physics/Line.h"

namespace IW {
	App::App() {
		InputManager->CreateDevice<Mouse>();
		InputManager->CreateDevice<RawKeyboard>();

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

		Ray2 r ({ -1, -1,}, {1, 1});
		Ray2 r2({ -1, -1,}, {1, 1});

		iw::vector2 i;
		int ib = r.Intersects(r2, &i);

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
