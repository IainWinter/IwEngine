#include "App.h"
#include "Layers/SpaceSandLayer.h"

App::App() : iw::Application() {
	PushLayer<iw::SpaceSandLayer>();
}

int App::Initialize(
	iw::InitOptions& options)
{
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

	return Application::Initialize(options);
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
