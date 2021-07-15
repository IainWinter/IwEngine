#include "App.h"
#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "iw/engine/Systems/PhysicsSystem.h"

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
		unsigned* colors = (unsigned*)tex->CreateColors();

		iw::Color black = iw::Color(0, 0, 0);
		iw::Color pink  = iw::Color::From255(235, 52, 186);

		bool f = false;
		for (int x = 0; x < tex->Width() ; x++)
		for (int y = 0; y < tex->Height(); y++) 
		{
			colors[x + y * tex->Width()] = f ? black.to32() : pink.to32();
			f = !f;
		}

		player = sand->MakeTile(tex, true);

		player.Find<iw::Rigidbody>()->AngularVelocity.z = 1;

		PushSystem<iw::PhysicsSystem>();

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
		800/**4/3*/,
		800/**4/3*/,
		false,
		iw::DisplayState::NORMAL
	};

	return new App();
}

iw::Application* GetApplicationForEditor() {
	return new App();
}
