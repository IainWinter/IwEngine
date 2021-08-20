#include "App.h"
#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/engine/Systems/EntityCleanupSystem.h"
#include "iw/physics/Dynamics/ImpulseSolver.h"
#include "iw/physics/Dynamics/SmoothPositionSolver.h"
#include "iw/math/noise.h"

#include "Systems/World_System.h"
#include "Systems/Item_System.h"

#include "Systems/Player_System.h"
#include "Systems/Enemy_System.h"

#include "Systems/Flocking_System.h"
#include "Systems/Projectile_System.h"

struct GameLayer : iw::Layer
{
	iw::SandLayer* sand;
	iw::SandLayer* sand_ui_laserCharge;
	
	iw::Entity player;
	iw::Mesh ui_background;
	iw::Mesh ui_player;
	iw::Mesh ui_score;

	GameLayer(
		iw::SandLayer* sand,
		iw::SandLayer* sand_ui_laserCharge
	) 
		: iw::Layer("Space game")
		, sand(sand)
		, sand_ui_laserCharge(sand_ui_laserCharge)
	{}

	int Initialize() override
	{
		ProjectileSystem* projectile_s = new ProjectileSystem(sand);
		PlayerSystem*     player_s     = new PlayerSystem(sand);
		
		PushSystem(projectile_s);
		PushSystem(player_s);
		PushSystem<EnemySystem>(sand);
		PushSystem<WorldSystem>(sand, player_s->player);
		PushSystem<ItemSystem> (sand, player_s->player);

		PushSystem<FlockingSystem>();

		PushSystem<iw::PhysicsSystem>();
		PushSystem<iw::EntityCleanupSystem>();

		Renderer->Device->SetClearColor(0, 0, 0, 0);

		Physics->AddSolver(new iw::SmoothPositionSolver());
		Physics->AddSolver(new iw::ImpulseSolver());

		player = player_s->player;

		// temp ui
		
		iw::ref<iw::Material> matp = REF<iw::Material>(
			Asset->Load<iw::Shader>("shaders/texture_cam.shader")
		);
		matp->Set("useAlpha", 1);
		matp->Set("alphaThresh", .9f);

		{ // player
			int w = player.Find<iw::Tile>()->m_sprite.Width();
			int h = player.Find<iw::Tile>()->m_sprite.Height();

			iw::ref<iw::Texture> tex = REF<iw::Texture>(w, h);
			tex->SetFilter(iw::NEAREST);
			tex->CreateColors();
			iw::ref<iw::Material> mat = matp->MakeInstance();
			mat->SetTexture("texture", tex);
			Renderer->Now->InitShader(mat->Shader, iw::CAMERA);
		
			ui_player = iw::ScreenQuad();
			ui_player.Material = mat;
		}

		{ // background
			//iw::ref<iw::Material> mat = REF<iw::Material>(
			//	Asset->Load<iw::Shader>("shaders/color_cam.shader")
			//);
			//mat->Set("color", iw::Color::From255(49, 49, 49));

			iw::ref<iw::Texture> tex = Asset->Load<iw::Texture>("textures/SpaceGame/ui_background.png");
			tex->SetFilter(iw::NEAREST);
			iw::ref<iw::Material> mat = matp->MakeInstance();
			mat->SetTexture("texture", tex);
			Renderer->Now->InitShader(mat->Shader, iw::CAMERA);

			ui_background = iw::ScreenQuad();
			ui_background.Material = mat;
		}

		{
			cam = new iw::OrthographicCamera(2, 2, -10, 10);
			cam->SetRotation(glm::angleAxis(iw::Pi, glm::vec3(0, 1, 0)));
		}


		if (int e = Layer::Initialize())
		{
			return e;
		}

		auto [w, h] = sand_ui_laserCharge->GetSandTexSize();
		for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
		{
			iw::Cell c = iw::Cell::GetDefault(iw::CellType::ROCK);
			sand_ui_laserCharge->m_world->SetCell(x, y, c);
		}

		for (int y = 0;         y <  h;         y++)
		for (int x = w / 2 - y; x <= w / 2 + y; x++)
		{
			iw::Cell c = iw::Cell::GetDefault(iw::CellType::EMPTY);
			sand_ui_laserCharge->m_world->SetCell(x, y, c);
		}

		return 0;
	}

	// temp ui
	iw::Camera* cam;
	int laserFluidToCreate = 0;
	int laserFluidToRemove = 0;
	int laserFluidCount = 0;
	glm::vec3 laserFluidVel = glm::vec3(0.f);

	void Update() override { // this is one frame behind, add a callback to the sand layer that gets called at the right time, right after rendering the world...
		iw::Texture& playerSprite = player.Find<iw::Tile>()->m_sprite;
		iw::Texture& uiPlayerTex  = *ui_player.Material->GetTexture("texture");

		unsigned* colorsFrom = playerSprite.Colors32();
		unsigned* colorsTo   = uiPlayerTex.Colors32();

		for (int i = 0; i < playerSprite.ColorCount32(); i++)
		{
			unsigned color = colorsFrom[i];
			iw::Tile::PixelState state = iw::Tile::cState(color);

			if (state == iw::Tile::PixelState::FILLED)
			{
				colorsTo[i] = color;
			}

			else {
				colorsTo[i] = 0;
			}
		}

		uiPlayerTex.Update(Renderer->Device);

		iw::SandChunk* chunk = sand_ui_laserCharge->m_world->GetChunk(0, 0);

		laserFluidVel = iw::lerp(laserFluidVel, player.Find<iw::Rigidbody>()->Velocity, iw::DeltaTime() * 5);

		LOG_INFO << laserFluidVel.x << ", " << laserFluidVel.y;

		std::pair<int, int> xy;

		for (int y = 0; y < chunk->m_height; y++)
		for (int x = 0; x < chunk->m_width;  x++)
		{
			chunk->KeepAlive(x, y);
			chunk->GetCell(x, y).dx = laserFluidVel.x;
			chunk->GetCell(x, y).dy = laserFluidVel.y;

			if (x > 12 && x < 20)
			{
				chunk->GetCell(x, y).dx = laserFluidVel.x * .5;
				chunk->GetCell(x, y).dy = -2;
			}
		}

		if (laserFluidToCreate > 0)
		{
			iw::Cell laserFluid = iw::Cell::GetDefault(iw::CellType::WATER);
			laserFluid.Color = iw::Color::From255(255, 38, 38);

			auto [w, h] = sand_ui_laserCharge->GetSandTexSize();
			int x = 0/*w / 2*/;
			int y = h - 1;

			if (sand_ui_laserCharge->m_world->IsEmpty(x, y))
			{
				sand_ui_laserCharge->m_world->SetCell(x, y, laserFluid);
				laserFluidToCreate--;
				laserFluidCount++;
			}
		}

		if (laserFluidToRemove > 0)
		{
			auto [w, h] = sand_ui_laserCharge->GetSandTexSize();
			int x = w / 2;
			int y = 0;

			if (!sand_ui_laserCharge->m_world->IsEmpty(x, y))
			{
				sand_ui_laserCharge->m_world->SetCell(x, y, iw::Cell::GetDefault(iw::CellType::EMPTY));
				laserFluidToRemove--;
				laserFluidCount--;
			}
		}

		LOG_INFO << laserFluidToRemove << " " << laserFluidCount;

		if (laserFluidCount == 0)
		{
			laserFluidToRemove = 0;
		}

		player.Find<Player>()->can_fire_laser = laserFluidCount > 0;
	}
	// end temp ui

	// could put in own system
	bool On(iw::ActionEvent& e) override
	{
		if (e.Action == CHANGE_LASER_FLUID)
		{
			ChangeLaserFluid_Event& event = e.as<ChangeLaserFluid_Event>();
			if (event.Amount > 0) laserFluidToCreate +=  event.Amount;
			if (event.Amount < 0) laserFluidToRemove += -event.Amount;
		}

		return Layer::On(e);
	}

	// end could put in own system, a fix between ui and functionality

	void PostUpdate() override
	{
		sand->m_drawMouseGrid = iw::Keyboard::KeyDown(iw::SHIFT);

		// screen - fills screen
		//		game board - fills empty space, 1:1 aspect ratio
		//		menu - y = 200px, x = width of game board
		//			player ui
		//			laser charges
		//			score
		//			time

		// constant constraints



		float width  = Renderer->Width();
		float height = Renderer->Height();

		float c_menu_height = height * .2;                    // 200px height
		float c_menu_position_y = -height + c_menu_height;  // fixed to bottom of screen

		float c_game_height     = -c_menu_position_y;       // fill rest of screen height
		float c_game_width      = c_game_height;			// keep width 1:1 ratio
		float c_game_position_y = c_menu_height;			// centered in rest of space
		float c_menu_width      = c_game_width;			    // menu width = game width

		float c_menu_padding = c_menu_height * .1;

		glm::vec2 health_dim = player.Find<iw::Tile>()->m_sprite.Dimensions();
		float c_health_aspect = health_dim.x / health_dim.y;

		float c_health_position_y = c_menu_position_y; // health is v-centered in menu
		float c_health_height = c_menu_height - c_menu_padding * 2;		// center has padding on both sides
		float c_health_width = c_health_height * c_health_aspect;	// keep width to aspect of texture

		float c_laser_height = c_menu_height - c_menu_padding * 2;
		float c_laser_width  = c_laser_height;
		float c_laser_position_x = -c_menu_width + c_laser_width + c_menu_padding * 2;
		float c_laser_position_y = c_menu_position_y;

		// SCALING

		float game_scale_x    = c_game_width      / width;
		float game_scale_y    = c_game_height     / height;
		float game_position_y = c_game_position_y / height;

		float menu_scale_x    = c_menu_width      / width;
		float menu_scale_y    = c_menu_height     / height;
		float menu_position_y = c_menu_position_y / height;

		float health_scale_x    = c_health_width      / width;
		float health_scale_y    = c_health_height     / height;
		float health_position_y = c_health_position_y / height;

		float laser_scale_x    = c_laser_width      / width;
		float laser_scale_y    = c_laser_height     / height;
		float laser_position_x = c_laser_position_x / width;
		float laser_position_y = c_laser_position_y / height;

		iw::Transform sandTransform;
		sandTransform.Position.y = game_position_y;
		sandTransform.Scale   .x = game_scale_x;
		sandTransform.Scale   .y = game_scale_y;

		iw::Transform uiPlayerTransform;
		uiPlayerTransform.Position.y = health_position_y;
		uiPlayerTransform.Scale   .x = health_scale_x;
		uiPlayerTransform.Scale   .y = health_scale_y;

		iw::Transform uiLaserChargeTransform;
		uiLaserChargeTransform.Position.x = laser_position_x;
		uiLaserChargeTransform.Position.y = laser_position_y;
		uiLaserChargeTransform.Scale.x    = laser_scale_x;
		uiLaserChargeTransform.Scale.y    = laser_scale_y;

		iw::Transform uiBackgroundTransform;
		uiBackgroundTransform.Position.z = -1;
		uiBackgroundTransform.Position.y = menu_position_y;
		uiBackgroundTransform.Scale   .x = menu_scale_x;
		uiBackgroundTransform.Scale   .y = menu_scale_y;

		// END SCALING

		Renderer->BeginScene(cam);
		Renderer->DrawMesh(uiPlayerTransform, ui_player);
		Renderer->DrawMesh(uiBackgroundTransform, ui_background);
		Renderer->DrawMesh(uiLaserChargeTransform, sand_ui_laserCharge->GetSandMesh());
		Renderer->EndScene();

		Renderer->BeginScene();
		Renderer->DrawMesh(sandTransform, sand->GetSandMesh());
		Renderer->EndScene();
	}
};

App::App() : iw::Application() 
{
	int cellSize  = 2;
	int cellMeter = 1;
	bool drawWithMouse = false;
	
	iw::SandLayer* sand = new iw::SandLayer(cellSize, cellMeter, 800, 800, 4, 4, drawWithMouse);

	// UI charge as liquid in a tank?
	iw::SandLayer* sand_ui_laser = new iw::SandLayer(cellSize, cellMeter, 64, 64);
	
	sand_ui_laser->m_updateDelay = .016f;

	PushLayer(sand);
	PushLayer(sand_ui_laser);
	PushLayer<GameLayer>(sand, sand_ui_laser);
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
	//options.AssetRootPath = "assets/";

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
