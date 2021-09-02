#include "App.h"
#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/engine/Systems/EntityCleanupSystem.h"
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
#include "Systems/Score_System.h"

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
	ScoreSystem*           score_s;

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

		score_s = new ScoreSystem();

		PushSystemFront(projectile_s);
		PushSystemFront(item_s);
		PushSystemFront(enemy_s);
		PushSystemFront(player_s);

		PushSystem(score_s);
		PushSystem<CorePixelsSystem>();

		PushSystem<FlockingSystem>();
		PushSystem<iw::PhysicsSystem>();
		PushSystem<iw::EntityCleanupSystem>();

		Bus->push<RunGame_Event>();

		m_cursor = sand->MakeTile(A_texture_ui_cursor);
		m_cursor.Find<iw::Tile>()->m_zIndex = 1;
		m_cursor.Find<iw::CollisionObject>()->IsTrigger = true;

		Physics->RemoveCollisionObject(m_cursor.Find<iw::CollisionObject>());

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

	// todo: make a ui system!!!!

	int CachedScore = -1;

	std::string itos(int numb)
	{
		std::stringstream buf;
		if (numb >= 0) buf << numb;
		else           buf << " "; // need space for UpdateMesh

		return buf.str();
	}

	//struct UIConstraint 
	//{
	//	virtual float Get(int parent) const = 0;
	//	virtual ~UIConstraint() = default;
	//};

	//struct Fixed : UIConstraint
	//{
	//	float value;

	//	Fixed(float value_) {
	//		value = value_;
	//	}

	//	float Get(int parent) const override
	//	{
	//		return value;
	//	}
	//};

	//struct Ratio : UIConstraint
	//{
	//	float value;

	//	Ratio(float value) : value(value) {}

	//	float Get(int parent) const override
	//	{
	//		return parent * value;
	//	}
	//};

	struct UI
	{
		float x, y, z, width, height;
		iw::Mesh mesh;

		UI(const iw::Mesh& mesh_)
		{
			mesh = mesh_;
			x = 0;
			y = 0;
			z = 0;
			width = 0;
			height = 0;
		}

		iw::Transform GetTransform(int screenWidth, int screenHeight, int screenDepth)
		{
			iw::Transform transform;
			transform.Position.x = floor(x)      / screenWidth;
			transform.Position.y = floor(y)      / screenHeight;
			transform.Position.z = -z / screenDepth;
			transform.Scale.x    = floor(width)  / screenWidth;
			transform.Scale.y    = floor(height) / screenHeight;

			return transform;
		}
	};

	struct UIScreen
	{
		std::vector<UI*> elements;
		int width, height, depth;

		UIScreen(int width_, int height_, int depth_)
		{
			width  = width_;
			height = height_;
			depth = depth_;
		}

		~UIScreen()
		{
			for (UI* ui : elements)
			{
				delete ui;
			}
		}

		UI* CreateElement(const iw::Mesh& mesh)
		{
			return elements.emplace_back(new UI(mesh));
		}

		void AddElement(UI* element)
		{
			elements.push_back(element);
		}

		void Draw(iw::Camera* camera, iw::ref<iw::QueuedRenderer>& renderer)
		{
			renderer->BeginScene(camera);

			for (UI* ui : elements)
			{
				renderer->DrawMesh(ui->GetTransform(width, height, depth), ui->mesh);
			}

			renderer->EndScene();
		}
	};


	void PostUpdate() override
	{
		if (m_player.Alive()) 
		{
			int ammo = m_player.Find<Player>()->CurrentWeapon->Ammo;

			A_font_arial->UpdateMesh(A_mesh_ui_text_ammo, itos(ammo), 12);

			CorePixels* core = m_player.Find<CorePixels>();
			float death = core->Timer / core->TimeWithoutCore;
			float red = 1 - death;//(cos(iw::TotalTime() * 1.5f * death) + 1) / 2;

			A_mesh_ui_background  .Material->Set("color", iw::Color(1, red, red));
			A_mesh_ui_playerHealth.Material->Set("color", iw::Color(1, red, red));
			uiJitterAmount += death * 10;
		}

		if (score_s->Score != CachedScore)
		{
			CachedScore = score_s->Score;
			A_font_arial->UpdateMesh(A_mesh_ui_text_score, itos(CachedScore), 9);
		}

		sand->m_drawMouseGrid = iw::Keyboard::KeyDown(iw::SHIFT);


		if (iw::Keyboard::KeyDown(iw::SHIFT))
		{
			Bus->push<EndGame_Event>();
		}

		uiJitterAmount = iw::lerp(uiJitterAmount, 0.f, iw::DeltaTime() * 10);

		// screen - fills screen
		//		game board - fills empty space, 1:1 aspect ratio
		//		menu - y = 200px, x = width of game board
		//			m_player ui
		//			laser charges
		//			score
		//			time

		// constant constraints

		UIScreen screen = UIScreen(Renderer->Width(), Renderer->Height(), 10);

		UI* menu = screen.CreateElement(A_mesh_ui_background);
		UI* game = screen.CreateElement(sand->GetSandMesh());

		menu->z = 1;
		game->z = 0;

		menu->height = screen.height * .2f;                                    // Ratio(.2f)
		menu->x = iw::randf() * uiJitterAmount;                                // Random(uiJitterAmount)
		menu->y = iw::randf() * uiJitterAmount - screen.height + menu->height; //Combine(Anchor(BOTTOM), menu->height, Random(uiJitterAmount))

		game->y = menu->height;                      // Same(manu->height)
		game->height = screen.height - menu->height; // fill rest of screen y
		game->width  = game->height;                 // Same(game->height)

		menu->width = game->width; // Same(game->width)

		float menu_pad = menu->height * .2f;

		UI* health = screen.CreateElement(A_mesh_ui_playerHealth);
		UI* laser = screen.CreateElement(sand_ui_laserCharge->GetSandMesh());

		health->z = -2; // ?? why is this negitive
		laser->z = 2;

		health->height = menu->height - menu_pad;
		health->width = health->height; // Ratio(health->height)
		health->x = menu->x;            // Same(menu->x)
		health->y = menu->y;            // Same(menu->y)

		laser->height = menu->height - menu_pad;
		laser->width = laser->height;                               // Same (laser->height)
		laser->x = menu->x - menu->width + laser->width + menu_pad; // Combine(Anchor(LEFT), laser->width, menu_pads)
		laser->y = menu->y;

		float score_pad = 30*8;

		UI* score = screen.CreateElement(A_mesh_ui_text_score);

		score->width  = screen.height;
		score->height = screen.height; // Same(score->width)
		score->x = menu->x + menu->width - score_pad;
		score->y = menu->y + menu->height - menu_pad;
		score->z = -3;

		if (showGameOver)
		{
			UI* gameover = screen.CreateElement(A_mesh_ui_text_gameOver);
			gameover->width = screen.width;
			gameover->height = screen.width;
		}

		screen.Draw(cam, Renderer);

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

	//GetDeviceCaps(Window(),, LOGPIXELSX)

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
