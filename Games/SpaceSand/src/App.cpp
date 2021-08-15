#include "App.h"
#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/engine/Systems/EntityCleanupSystem.h"
#include "iw/physics/Dynamics/ImpulseSolver.h"
#include "iw/physics/Dynamics/SmoothPositionSolver.h"
#include "iw/math/noise.h"

#include "Systems/World_System.h"

#include "Systems/Player_System.h"
#include "Systems/Enemy_System.h"

#include "Systems/Flocking_System.h"
#include "Systems/Projectile_System.h"

struct GameLayer : iw::Layer
{
	iw::SandLayer* sand;
	
	GameLayer(
		iw::SandLayer* sand
	) 
		: iw::Layer("Space game")
		, sand(sand)
	{}

	int Initialize() override
	{
		ProjectileSystem* projectile_s = new ProjectileSystem(sand);
		PlayerSystem*     player_s     = new PlayerSystem(sand);

		PushSystem(projectile_s);
		PushSystem(player_s);
		PushSystem<EnemySystem>(sand);
		PushSystem<WorldSystem>(sand, player_s->player);

		PushSystem<FlockingSystem>();

		Renderer->Device->SetClearColor(0, 0, 0, 0);

		Physics->AddSolver(new iw::SmoothPositionSolver());
		Physics->AddSolver(new iw::ImpulseSolver());

		PushSystem<iw::PhysicsSystem>();
		PushSystem<iw::EntityCleanupSystem>();

		return Layer::Initialize();
	}

	void PostUpdate() override
	{
		float height = Renderer->Height();

		float sandSize = sand->GetSandTexSize2().second * sand->m_cellSize * 2;
		float menuSize = height - sandSize;


		float sandPos   = menuSize / height;
		float sandScale = sandSize / height;

		iw::Transform sandTransform;
		sandTransform.Position.y = sandPos;
		sandTransform.Scale   .y = sandScale;

		Renderer->BeginScene();
		Renderer->DrawMesh(sandTransform, sand->GetSandMesh());
		Renderer->EndScene();
	}
};

App::App() : iw::Application() 
{
	int cellSize  = 2;
	int cellMeter = 1;

	iw::SandLayer* sand = new iw::SandLayer(cellSize, cellMeter, 800, 800, true);

	PushLayer(sand);
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
		800,
		950,
		true,
		iw::DisplayState::NORMAL
	};

	return new App();
}

iw::Application* GetApplicationForEditor() {
	return new App();
}
