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
#include "Systems/Weapon_System.h"

#include "Systems/Background_System.h"

#include "iw/graphics/Font.h"

struct GameLayer : iw::Layer
{
	iw::SandLayer* sand;
	iw::SandLayer* sand_ui_laserCharge;
	
	BackgroundSystem* background_s;

	iw::Entity player;
	iw::Mesh ui_score;
	iw::ref<iw::Font> ui_font;

	iw::Entity cursor;

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
		// load all assets into global variables
		if (int e = LoadAssets(Asset.get(), Renderer->Now.get()))
		{
			return e;
		}

		cursor = sand->MakeTile(A_texture_ui_cursor);
		cursor.Find<iw::CollisionObject>()->Transform.Scale = glm::vec3(.25f);
		cursor.Find<iw::Tile>()->m_zIndex = -10;

		Renderer->Device->SetClearColor(0, 0, 0, 0);

		Physics->AddSolver(new iw::SmoothPositionSolver());
		Physics->AddSolver(new iw::ImpulseSolver());

		//if(false){ // font
		//	iw::ref<iw::Material> fontMat = REF<iw::Material>(
		//		Asset->Load<iw::Shader>("shaders/font.shader")
		//	);
		//	Renderer->Now->InitShader(fontMat->Shader, iw::CAMERA);

		//	ui_font = Asset->Load<iw::Font>("fonts/arial.fnt");
		//	ui_score = ui_font->GenerateMesh("123\n123\n123\n123\n123", .001f, 1);
		//	ui_score.Material = fontMat;
		//}

		{
			cam = new iw::OrthographicCamera(2, 2, -10, 10);
			cam->SetRotation(glm::angleAxis(iw::Pi, glm::vec3(0, 1, 0)));
		}

		// more temp ui

		{ // laser fluid box
			iw::SandChunk* chunk = sand_ui_laserCharge->m_world->GetChunk(0, 0); // only 1 chunk in this
			auto [w, h] = sand_ui_laserCharge->GetSandTexSize();
			
			std::vector<int> widths = {
				4, 4,
				6, 
				8, 
				14, 
				24, 
				30, 
				34, 
				38, 38, 38,
				40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
				38, 38, 38,
				36, 36,
				34,
				30,
				24
			};

			std::vector<std::pair<int, int>> fills = { // these are slightly wrong
				{32, 36},
				{32, 37},

				{33, 33},
				{33, 34},
				{33, 35},
				{33, 36},
				
				{34, 32},
				{34, 33},
				{34, 34},
				{34, 35},
				{34, 36},
				
				{35, 31},
				{35, 32},
				{35, 33},
				{35, 34},
				{35, 35},
				{35, 36},
				{35, 37},
				
				{36, 33},
				{36, 34},
				{36, 35},
				{36, 36},
				{36, 37},

				{37, 33},
				{37, 34},
				{37, 35},
				{37, 36},
				{37, 37},
				{37, 38},

				{38, 34},
				{38, 35},
				{38, 36},
				{38, 37},
				{38, 38},
				{38, 39},

				{39, 35},
				{39, 36},
				{39, 37},
				{39, 38},

				{40, 35},
				{40, 36},
			};

			for (int y = 0; y < h; y++)
			for (int x = 0; x < w; x++)
			{
				chunk->SetCell(x, y, true, iw::SandField::SOLID);
			}

			for (int y = 0; y < widths.size(); y++)
			{
				int yw = widths[y] / 2;

				for (int x = w/2 - yw; x < w/2 + yw; x++)
				{
					chunk->SetCell(x, y, false, iw::SandField::SOLID);
				}
			}

			for (const auto& [x, y] : fills)
			{
				chunk->SetCell(x-1, y-1, true, iw::SandField::SOLID); // this is slightly wrong
			}
		}

		for (int i = 0; i < 0; i++)
		{
			iw::Entity asteroid = sand->MakeTile(A_texture_asteroid, true);
			iw::Transform* transform = asteroid.Find<iw::Transform>();
			iw::Rigidbody* rigidbody = asteroid.Find<iw::Rigidbody>();

			transform->Position.y = 400;
			transform->Position.x = 400 * iw::randf();
			rigidbody->SetTransform(transform);

			rigidbody->Velocity.y = -10 * iw::randf() - 5;
			rigidbody->Velocity.x = 10 * iw::randf();
			rigidbody->AngularVelocity.z = .2;

			rigidbody->SetMass(10000);
		}

		PlayerSystem* player_s = PushSystem<PlayerSystem>(sand);
		PopSystem(player_s); // to assign app vars
		
		if (int e = player_s->Initialize())
		{
			return e;
		}

		player = player_s->player;

		//background_s = PushSystem<BackgroundSystem>();
		
		PushSystem<WeaponSystem>();
		PushSystem<ProjectileSystem>(sand);
		PushSystem<EnemySystem>     (sand);
		PushSystem<WorldSystem>     (sand, player_s->player);
		PushSystem<ItemSystem>      (sand, player_s->player);

		PushSystem<FlockingSystem>();

		PushSystem<iw::PhysicsSystem>();
		PushSystem<iw::EntityCleanupSystem>();

		if (int e = Layer::Initialize())
		{
			return e;
		}

		PushSystem(player_s);

		return 0;
	}

	// temp ui
	iw::Camera* cam;
	int laserFluidToCreate = 0;
	int laserFluidToRemove = 0;
	int laserFluidCount = 0;
	glm::vec3 laserFluidVel = glm::vec3(0.f);
	float canFireTimer = 0;

	float uiJitterAmount = 0;

	void Update() override { // this is one frame behind, add a callback to the sand layer that gets called at the right time, right after rendering the world...
		
		cursor.Find<iw::Transform>()->Position = glm::vec3(sand->sP, 0.f);

		iw::Texture& playerSprite = player.Find<iw::Tile>()->m_sprite;
		iw::ref<iw::Texture> uiPlayerTex  = A_mesh_ui_playerHealth.Material->GetTexture("texture");

		unsigned* colorsFrom = playerSprite.Colors32();
		unsigned* colorsTo   = uiPlayerTex->Colors32();

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

		uiPlayerTex->Update(Renderer->Device);

		iw::SandChunk* chunk = sand_ui_laserCharge->m_world->GetChunk(0, 0);

		laserFluidVel = iw::lerp(laserFluidVel, player.Find<iw::Rigidbody>()->Velocity, iw::DeltaTime() * 5);

		std::pair<int, int> xy;

		for (int y = 0; y < chunk->m_height; y++)
		for (int x = 0; x < chunk->m_width;  x++)
		{
			chunk->KeepAlive(x, y);
			chunk->GetCell(x, y).dx = -laserFluidVel.x / 5; // this should match 'acceleration' even tho its instant
			chunk->GetCell(x, y).dy = -laserFluidVel.y / 5;
		}

		if (laserFluidToCreate > 0)
		{
			iw::Cell laserFluid = iw::Cell::GetDefault(iw::CellType::WATER);
			laserFluid.Color = iw::Color::From255(255, 38, 38);

			auto [w, h] = sand_ui_laserCharge->GetSandTexSize();
			int x = 31;
			int y = 31;

			if (sand_ui_laserCharge->m_world->IsEmpty(x, y))
			{
				sand_ui_laserCharge->m_world->SetCell(x, y, laserFluid);
				laserFluidToCreate--;
				laserFluidCount++;
			}
		}

		if (/*laserFluidToRemove > 0*/player.Find<Player>()->i_fire2)
		{
			auto [w, h] = sand_ui_laserCharge->GetSandTexSize();
			for (int x = 18; x < 22; x++)
			{
				int y = 0;

				if (!sand_ui_laserCharge->m_world->IsEmpty(x, y))
				{
					sand_ui_laserCharge->m_world->SetCell(x, y, iw::Cell::GetDefault(iw::CellType::EMPTY));
					laserFluidToRemove--;
					laserFluidCount--;

					canFireTimer += .01f;
				}
			}
		}

		if (laserFluidCount == 0)
		{
			laserFluidToRemove = 0;
		}

		canFireTimer = iw::max(canFireTimer - iw::DeltaTime(), 0.f);

		player.Find<Player>()->can_fire_laser = canFireTimer > 0.f;
	}
	// end temp ui

	// could put in own system
	bool On(iw::ActionEvent& e) override
	{
		switch (e.Action)
		{
			case CHANGE_LASER_FLUID:
			{
				ChangeLaserFluid_Event& event = e.as<ChangeLaserFluid_Event>();
				if (event.Amount > 0) laserFluidToCreate +=  event.Amount;
				if (event.Amount < 0) laserFluidToRemove += -event.Amount;
				
				break;
			}

			case PROJ_HIT_TILE:
			{
				ProjHitTile_Event& event = e.as<ProjHitTile_Event>();
				if (event.Hit.Has<Player>())
				{
					uiJitterAmount = 75;
					laserFluidVel.y -= 200;
					laserFluidVel.x -= 75 * iw::randfs();
				}

				break;
			}
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

		float c_menu_height = floor(height * .2);                    // 200px height
		float c_menu_position_y = -height + c_menu_height;  // fixed to bottom of screen
		float c_menu_position_x = 0;
		
		// damage jitter

		c_menu_position_x += floor(iw::randf() * uiJitterAmount); // should this be symmetric or only top left?
		c_menu_position_y += floor(iw::randf() * uiJitterAmount);
		uiJitterAmount = iw::lerp(uiJitterAmount, 0.f, iw::DeltaTime() * 10);

		// end damage jitter

		float c_game_height     = height - c_menu_height;       // fill rest of screen height
		float c_game_width      = c_game_height;			// keep width 1:1 ratio
		float c_game_position_y = c_menu_height;			// centered in rest of space
		float c_menu_width      = c_game_width;			    // menu width = game width

		float c_menu_padding = floor(c_menu_height * .1);

		glm::vec2 health_dim = player.Find<iw::Tile>()->m_sprite.Dimensions();
		float c_health_aspect = health_dim.x / health_dim.y;

		float c_health_position_x = c_menu_position_x; // health is centered in menu
		float c_health_position_y = c_menu_position_y;
		float c_health_height = c_menu_height - c_menu_padding * 2;  // center has padding on both sides
		float c_health_width  = c_health_height * c_health_aspect;	 // keep width to aspect of texture

		float c_laser_height = c_menu_height - c_menu_padding * 2;
		float c_laser_width  = c_laser_height;
		float c_laser_position_x = c_menu_position_x - c_menu_width + c_laser_width + c_menu_padding * 2;
		float c_laser_position_y = c_menu_position_y;
		
		// SCALING

		float game_scale_x    = c_game_width      / width;
		float game_scale_y    = c_game_height     / height;
		float game_position_y = c_game_position_y / height;

		float menu_scale_x    = c_menu_width      / width;
		float menu_scale_y    = c_menu_height     / height;
		float menu_position_x = c_menu_position_x / width;
		float menu_position_y = c_menu_position_y / height;

		float health_scale_x    = c_health_width      / width;
		float health_scale_y    = c_health_height     / height;
		float health_position_x = c_health_position_x / width;
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
		uiPlayerTransform.Position.x = health_position_x;
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
		uiBackgroundTransform.Position.x = menu_position_x;
		uiBackgroundTransform.Position.y = menu_position_y;
		uiBackgroundTransform.Scale   .x = menu_scale_x;
		uiBackgroundTransform.Scale   .y = menu_scale_y;

		// END SCALING

		Renderer->BeginScene(cam);
		//Renderer->DrawMesh(iw::Transform(glm::vec3(0.f), glm::vec3(1, width/height, 1)), background_s->m_stars.GetParticleMesh());

		Renderer->DrawMesh(uiPlayerTransform, A_mesh_ui_playerHealth);
		Renderer->DrawMesh(uiBackgroundTransform, A_mesh_ui_background);
		Renderer->DrawMesh(uiLaserChargeTransform, sand_ui_laserCharge->GetSandMesh());
		//Renderer->DrawMesh(iw::Transform(), ui_score);
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
	iw::SandLayer* sand_ui_laser = new iw::SandLayer(1, 1, 40, 40);
	
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
	options.AssetRootPath = "C:/dev/wEngine/_assets/";

	options.WindowOptions = iw::WindowOptions {
		800,
		1000,
		false,
		iw::DisplayState::NORMAL
	};

	return new App();
}

iw::Application* GetApplicationForEditor() {
	return new App();
}
