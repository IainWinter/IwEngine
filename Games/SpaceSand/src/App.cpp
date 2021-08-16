#include "App.h"
#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/engine/Systems/EntityCleanupSystem.h"
#include "iw/physics/Dynamics/ImpulseSolver.h"
#include "iw/physics/Dynamics/SmoothPositionSolver.h"
#include "iw/math/noise.h"

#include "Systems/World_System.h"
#include "Systems/Health_System.h"

#include "Systems/Player_System.h"
#include "Systems/Enemy_System.h"

#include "Systems/Flocking_System.h"
#include "Systems/Projectile_System.h"

struct GameLayer : iw::Layer
{
	iw::SandLayer* sand;
	
	iw::Entity player;
	iw::Mesh ui_background;
	iw::Mesh ui_player;
	iw::Mesh ui_score;

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
		PushSystem<EnemySystem> (sand);
		PushSystem<WorldSystem> (sand, player_s->player);
		PushSystem<HealthSystem>(sand, player_s->player);

		PushSystem<FlockingSystem>();

		Renderer->Device->SetClearColor(0, 0, 0, 0);

		Physics->AddSolver(new iw::SmoothPositionSolver());
		Physics->AddSolver(new iw::ImpulseSolver());

		PushSystem<iw::PhysicsSystem>();
		PushSystem<iw::EntityCleanupSystem>();
		
		// temp ui
		
		{ // player
			iw::ref<iw::Texture> tex = REF<iw::Texture>(24, 24);
			tex->SetFilter(iw::NEAREST);
			tex->CreateColors();
			iw::ref<iw::Material> mat = REF<iw::Material>(
				Asset->Load<iw::Shader>("shaders/texture_cam.shader")
			);
			mat->SetTexture("texture", tex);
			Renderer->Now->InitShader(mat->Shader, iw::CAMERA);
		
			ui_player = iw::ScreenQuad();
			ui_player.Material = mat;
		}

		{ // background
			iw::ref<iw::Material> mat = REF<iw::Material>(
				Asset->Load<iw::Shader>("shaders/color_cam.shader")
			);
			mat->Set("color", iw::Color::From255(49, 49, 49));
		
			ui_background = iw::ScreenQuad();
			ui_background.Material = mat;
		}

		{
			cam = new iw::OrthographicCamera(1.6, 2, -10, 10);
			cam->SetRotation(glm::angleAxis(iw::Pi, glm::vec3(0, 1, 0)));
		}

		player = player_s->player;

		return Layer::Initialize();
	}

	// temp ui
	iw::Camera* cam;
	void Update() override { // this is one frame behind, add a callback to the sand layer that gets called at the right time, right after rendering the world...
		iw::ref<iw::Texture> uiPlayerTex = ui_player.Material->GetTexture("texture");
		unsigned* colors = uiPlayerTex->Colors32();
		int playerX = player.Find<iw::Transform>()->Position.x;
		int playerY = player.Find<iw::Transform>()->Position.y;
		for (int y = 0; y < uiPlayerTex->Height(); y++)
		for (int x = 0; x < uiPlayerTex->Width(); x++)
		{
			int px = playerX + x - uiPlayerTex->Width()  / 2;
			int py = playerY + y - uiPlayerTex->Height() / 2;

			if (iw::SandChunk* chunk = sand->m_world->GetChunk(px, py))
			{
				colors[x + y * uiPlayerTex->Width()] = chunk->GetCell<unsigned>(px, py, iw::SandField::COLOR);
			}
		}

		uiPlayerTex->Update(Renderer->Device);
	}
	// end temp ui

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

		// tmep ui 

		float menuPos = -sandScale;
		float menuScale = menuSize / height;

		float uiPlayerMargin = menuScale / 10;

		iw::Transform uiPlayerTransform;
		uiPlayerTransform.Position.y = menuPos - uiPlayerMargin;
		uiPlayerTransform.Scale.x = menuScale - uiPlayerMargin;
		uiPlayerTransform.Scale.y = menuScale - uiPlayerMargin;

		iw::Transform uiBackgroundTransform;
		uiBackgroundTransform.Position.z = -1;
		uiBackgroundTransform.Position.y = menuPos;
		uiBackgroundTransform.Scale.y = menuScale;

		Renderer->BeginScene(cam);
		Renderer->DrawMesh(uiBackgroundTransform, ui_background);
		Renderer->DrawMesh(uiPlayerTransform, ui_player);
		Renderer->EndScene();

		// end temp ui

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
		1000,
		true,
		iw::DisplayState::NORMAL
	};

	return new App();
}

iw::Application* GetApplicationForEditor() {
	return new App();
}
