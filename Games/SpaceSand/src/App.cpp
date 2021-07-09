#include "App.h"
#include "plugins/iw/Sand/Engine/SandLayer.h"

struct GameLayer : iw::Layer
{
	iw::SandLayer* sand;
	iw::Entity player;

	GameLayer(
		iw::SandLayer* sand
	) 
		: iw::Layer("Space game")
		, sand(sand)
	{}

	int Initialize() override 
	{
		iw::ref<iw::Texture> tex = REF<iw::Texture>(64, 64);
		tex->CreateColors();

		for (int y = 0; y < tex->Height(); y++)
		for (int x = 0; x < tex->Width();  x++)
		{
			tex->m_colors[x + y * tex->Width()] = iw::Color(1).to32();
		}

		player = sand->MakeTile(tex, true);

		return Layer::Initialize();
	}

	void PostUpdate() override
	{
		sand->SetCamera(0, 0, 10, 10);

		Renderer->BeginScene();
		Renderer->DrawMesh(iw::Transform(), sand->GetSandMesh());
		Renderer->EndScene();
	}
};

App::App() : iw::Application() 
{
	int cellSize = 4;
	int cellMeter = 10;

	iw::SandLayer* sand = PushLayer<iw::SandLayer>(cellSize, cellMeter);
						  PushLayer<GameLayer>(sand);
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
