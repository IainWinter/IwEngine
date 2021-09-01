#include "App.h"
#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/engine/Systems/EntityCleanupSystem.h"
#include "iw/engine/Systems/TransformCacheSystem.h"
#include "iw/physics/Dynamics/ImpulseSolver.h"
#include "iw/physics/Dynamics/SmoothPositionSolver.h"
#include "iw/math/noise.h"

#include "Systems/World_System.h"
#include "Systems/KeepInWorld_System.h"
#include "Systems/Item_System.h"

#include "Systems/Player_System.h"
#include "Systems/PlayerLaserTank_System.h"
#include "Systems/Enemy_System.h"
#include "Systems/CorePixels_System.h"

#include "Systems/Flocking_System.h"
#include "Systems/Projectile_System.h"

#include "iw/graphics/Font.h"

struct GameLayer : iw::Layer
{
	iw::SandLayer* sand;
	iw::SandLayer* sand_ui_laserCharge;
	
	iw::Entity m_player;
	iw::Entity m_cursor;

	PlayerSystem*          player_s;
	PlayerLaserTankSystem* playerTank_s;
	WorldSystem*           world_s;
	ProjectileSystem*      projectile_s;
	EnemySystem*           enemy_s;
	ItemSystem*            item_s;
	KeepInWorldSystem*     keepInWorld_s;

	bool showGameOver = false;

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
		auto [sandWidth, sandHeight] = sand->GetSandTexSize2();
		sand->SetCamera(sandWidth, sandHeight);

		// load all assets into global variables
		if (int e = LoadAssets(Asset.get(), Renderer->Now.get()))
		{
			return e;
		}

		Renderer->Device->SetClearColor(0, 0, 0, 1);

		Physics->AddSolver(new iw::SmoothPositionSolver());
		Physics->AddSolver(new iw::ImpulseSolver());

		{
			cam = new iw::OrthographicCamera(2, 2, -10, 10);
			cam->Transform.Rotation = glm::angleAxis(iw::Pi, glm::vec3(0, 1, 0));
		}

		projectile_s = new ProjectileSystem(sand);
		item_s       = new ItemSystem      (sand);
		world_s      = new WorldSystem     (sand);
		enemy_s      = new EnemySystem     (sand);
		player_s     = new PlayerSystem    (sand);
		playerTank_s = new PlayerLaserTankSystem(sand_ui_laserCharge);
		
		keepInWorld_s = new KeepInWorldSystem();

		PushSystemFront(projectile_s);
		PushSystemFront(item_s);
		PushSystemFront(enemy_s);
		PushSystemFront(player_s);

		PushSystem<CorePixelsSystem>();

		PushSystem<FlockingSystem>();
		PushSystem<iw::PhysicsSystem>();
		PushSystem<iw::EntityCleanupSystem>();
		PushSystem<iw::TransformCacheSystem>();

		Bus->push<RunGame_Event>();

		m_cursor = sand->MakeTile(A_texture_ui_cursor);
		m_cursor.Find<iw::Tile>()->m_zIndex = 1;
		m_cursor.Find<iw::CollisionObject>()->IsTrigger = true;

		return Layer::Initialize();
	}

	//std::unordered_map<iw::EntityHandle, std::pair<iw::Mesh, int>, iw::ehandle_hash> debug_tileColliders;
	//int tick = 0;

	// temp ui
	iw::Camera* cam;
	int laserFluidToCreate = 0;
	int laserFluidToRemove = 0;
	int laserFluidCount = 0;
	glm::vec3 laserFluidVel = glm::vec3(0.f);
	float canFireTimer = 0;
	float uiJitterAmount = 0;

	void FixedUpdate() override
	{
		Physics->debug__ScrambleObjects();
	}

	void Update() override { // this is one frame behind, add a callback to the sand layer that gets called at the right time, right after rendering the world...
	
		m_cursor.Find<iw::CollisionObject>()->Transform.Position = glm::vec3(sand->sP, 0.f);

		// state change :o

		if (showGameOver)
		{
			if (iw::Keyboard::KeyDown(iw::InputName::SPACE))
			{
				// could reset by popping all layers and pushing them again
				// each systems OnPush / OnPop could clean up and reint

				// now doing it with events END_GAME and RUN_GAME
				// i think that the push/pop was is better but this is simpler
				// ill have to design it better after this weekend when there are actual menus
				// there will have to be more state, which will require the systems being push around
				// maybe layers for menus too actually

				Bus->push<RunGame_Event>();
			}

			return;
		}

		//tick++;

		//Space->Query<iw::Tile>().Each([&](
		//	iw::EntityHandle entity, 
		//	iw::Tile* tile) 
		//{
		//	auto& [mesh, mtick] = debug_tileColliders[entity];

		//	mtick = tick;

		//	if (!mesh.Data)
		//	{
		//		iw::MeshDescription desc;
		//		desc.DescribeBuffer(iw::bName::POSITION, iw::MakeLayout<float>(2));

		//		mesh = (new iw::MeshData(desc))->MakeInstance();
		//		mesh.Data->SetBufferData(iw::bName::POSITION, tile->m_collider.size(), tile->m_collider.data());
		//		mesh.Data->SetIndexData(tile->m_colliderIndex.size(), tile->m_colliderIndex.data());
		//		mesh.Material = A_material_debug_wireframe;
		//	}
		//});

		//for (auto& itr = debug_tileColliders.begin(); itr != debug_tileColliders.end(); itr++)
		//{
		//	auto [mesh, mtick] = itr->second;
		//	if (mtick != tick)
		//	{
		//		debug_tileColliders.erase(itr); // not sure if this is undefined
		//	}
		//}

		iw::Texture& playerSprite = m_player.Find<iw::Tile>()->m_sprite;
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
	
		//if (iw::Mouse::ButtonDown(iw::RMOUSE))
		//{
		//	SpawnItem_Config config;
		//	config.X = sand->sP.x;
		//	config.Y = sand->sP.y;
		//	config.Item = ItemType::HEALTH;

		//	Bus->push<SpawnItem_Event>(config);
		//}
	}
	// end temp ui

	// could put in own system
	bool On(iw::ActionEvent& e) override
	{
		switch (e.Action)
		{
			case PROJ_HIT_TILE:
			{
				ProjHitTile_Event& event = e.as<ProjHitTile_Event>();
				if (event.Config.Hit.Has<Player>())
				{
					uiJitterAmount = 75;
				}

				break;
			}
			case END_GAME:
			{
				showGameOver = true;
				PopSystem(playerTank_s);
				PopSystem(world_s);
				PopSystem(keepInWorld_s);

				break;
			}
			case RUN_GAME: {
				showGameOver = false;
				PushSystemFront(playerTank_s);
				PushSystemFront(world_s);
				PushSystem(keepInWorld_s);

				for (int x = 0; x < 400; x++)
				for (int y = 0; y < 400; y++)
				{
					sand->m_world->SetCell(x, y, iw::Cell::GetDefault(iw::CellType::EMPTY));
				}

				break;
			}
			case CREATED_PLAYER: {
				m_player = e.as<CreatedPlayer_Event>().PlayerEntity;
				break;
			}
		}

		return Layer::On(e);
	}

	// end could put in own system, a fix between ui and functionality

	void PostUpdate() override
	{
		if (m_player.Alive()) 
		{
			int ammo = m_player.Find<Player>()->CurrentWeapon->Ammo;

			std::stringstream buf;
			if (ammo >= 0) buf << ammo;
			else           buf << " ";

			A_font_arial->UpdateMesh(A_mesh_ui_text_ammo, buf.str(), .001f, 1);

			CorePixels* core = m_player.Find<CorePixels>();
			float death = core->Timer / core->TimeWithoutCore;
			float red = 1 - death;//(cos(iw::TotalTime() * 1.5f * death) + 1) / 2;

			A_mesh_ui_background  .Material->Set("color", iw::Color(1, red, red));
			A_mesh_ui_playerHealth.Material->Set("color", iw::Color(1, red, red));
			uiJitterAmount += death * 10;
		}

		sand->m_drawMouseGrid = iw::Keyboard::KeyDown(iw::SHIFT);

		// screen - fills screen
		//		game board - fills empty space, 1:1 aspect ratio
		//		menu - y = 200px, x = width of game board
		//			m_player ui
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

		glm::vec2 health_dim = m_player.Find<iw::Tile>()->m_sprite.Dimensions();
		float c_health_aspect = health_dim.x / health_dim.y;

		float c_health_position_x = c_menu_position_x; // health is centered in menu
		float c_health_position_y = c_menu_position_y;
		float c_health_height = c_menu_height - c_menu_padding * 2;  // center has padding on both sides
		float c_health_width  = c_health_height * c_health_aspect;	 // keep width to aspect of texture

		float c_laser_height = c_menu_height - c_menu_padding * 2;
		float c_laser_width  = c_laser_height;
		float c_laser_position_x = c_menu_position_x - c_menu_width + c_laser_width + c_menu_padding * 2;
		float c_laser_position_y = c_menu_position_y;
		
		float c_ammoCount_position_x = c_menu_position_x + c_menu_width - 200;
		float c_ammoCount_position_y = c_menu_position_y + 100;

		//sand->m_world->SetCell(sand->sP.x, sand->sP.y, iw::Cell::GetDefault(iw::CellType::ROCK));

		//float c_cursor_position_x =  iw::Mouse::ClientPos().x() * 2 - width;
		//float c_cursor_position_y = -iw::Mouse::ClientPos().y() * 2 + height;
		//float c_cursor_width      = 20;
		//float c_cursor_height      = c_cursor_width;

		// SCALING, this should be done by the camera...

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

		float ammoCount_position_x = c_ammoCount_position_x / width;
		float ammoCount_position_y = c_ammoCount_position_y / height;

		//float cursor_position_x = c_cursor_position_x / width;
		//float cursor_position_y = c_cursor_position_y / height;
		//float cursor_width      = c_cursor_width      / width;
		//float cursor_height     = c_cursor_height     / height;

		iw::Transform sandTransform;
		sandTransform.Position.y = game_position_y;
		sandTransform.Scale   .x = game_scale_x;
		sandTransform.Scale   .y = game_scale_y;

		iw::Transform backgroundTransform = sandTransform;
		backgroundTransform.Position.z = -5;

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

		iw::Transform uiAmmoTextTransform;
		uiAmmoTextTransform.Position.x = ammoCount_position_x;
		uiAmmoTextTransform.Position.y = ammoCount_position_y;
		uiAmmoTextTransform.Scale.x = 1;
		uiAmmoTextTransform.Scale.y = width / height;

		//iw::Transform uiCursorTransform;
		//uiCursorTransform.Position.z = 1;
		//uiCursorTransform.Position.x = cursor_position_x;
		//uiCursorTransform.Position.y = cursor_position_y;
		//uiCursorTransform.Scale.x    = cursor_width;
		//uiCursorTransform.Scale.y    = cursor_height;

		// END SCALING

		Renderer->BeginScene();
		Renderer->DrawMesh(sandTransform, sand->GetSandMesh());
		Renderer->EndScene();

		Renderer->BeginScene(cam);
		//Renderer->DrawMesh(iw::Transform(glm::vec3(0.f), glm::vec3(1, width/height, 1)), background_s->m_stars.GetParticleMesh());

		//for (auto& [entity, data] : debug_tileColliders)
		//{
		//	iw::Transform transform = *Space->FindComponent<iw::Transform>(entity);
		//	transform.Position /= 200.f;
		//	transform.Position -= glm::vec3(1, 1, 0);
		//	transform.Position.z = 10;
		//	transform.Position.y += game_position_y;
		//	transform.Scale /= 200.f;

		//	transform.Scale.x *= c_game_width / width;
		//	transform.Scale.y *= c_game_height / height;

		//	Renderer->DrawMesh(transform, data.first);
		//}

		Renderer->DrawMesh(backgroundTransform, A_mesh_background);

		Renderer->DrawMesh(uiPlayerTransform, A_mesh_ui_playerHealth);
		Renderer->DrawMesh(uiBackgroundTransform, A_mesh_ui_background);
		Renderer->DrawMesh(uiLaserChargeTransform, sand_ui_laserCharge->GetSandMesh());
		Renderer->DrawMesh(uiAmmoTextTransform, A_mesh_ui_text_ammo);
		//Renderer->DrawMesh(uiCursorTransform, A_mesh_ui_cursor);
		
		if (showGameOver)
		{
			iw::Transform temp;
			temp.Scale.y = width / height;
			Renderer->DrawMesh(temp, A_mesh_ui_text_gameOver);
		}

		Renderer->EndScene();


		Physics->debug__ScrambleObjects();
	}
};

App::App() : iw::Application() 
{
	int cellSize  = 2;
	int cellMeter = 1;
	bool drawWithMouse = false;
	
	iw::SandLayer* sand = new iw::SandLayer(cellSize, cellMeter, 800, 800, 4, 4, drawWithMouse);
	sand->m_updateDelay = 1/144.f;

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
	//options.AssetRootPath = "C:/dev/IwEngine/_assets/";
	//options.AssetRootPath = "assets/";

	options.WindowOptions = iw::WindowOptions {
		800 + 38/2,
		1000 + 38,
		false,
		iw::DisplayState::NORMAL
	};

	return new App();
}

iw::Application* GetApplicationForEditor() {
	return new App();
}
