#include "App.h"

int GameLayer::Initialize()
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

	projectile_s  = new ProjectileSystem(sand);
	item_s        = new ItemSystem      (sand);
	world_s       = new WorldSystem     (sand);
	enemy_s       = new EnemySystem     (sand);
	player_s      = new PlayerSystem    (sand);
	explosion_s   = new ExplosionSystem (sand);
	playerTank_s  = new PlayerLaserTankSystem(sand_ui_laserCharge);
	score_s       = new ScoreSystem();
	corePixels_s  = new CorePixelsSystem();
	flocking_s    = new FlockingSystem();
	keepInWorld_s = new KeepInWorldSystem();

	PushSystem(projectile_s);
	PushSystem(item_s);
	PushSystem(enemy_s);
	PushSystem(player_s);
	PushSystem(score_s);
	PushSystem(corePixels_s);
	PushSystem(flocking_s);
	PushSystem(explosion_s);
	
	PushSystem<iw::PhysicsSystem>();
	PushSystem<iw::EntityCleanupSystem>();

	Bus->push<StateChange_Event>(RUN_STATE);

	m_cursor = sand->MakeTile(A_texture_ui_cursor);
	m_cursor.Find<iw::Tile>()->m_zIndex = 1;
	m_cursor.Find<iw::CollisionObject>()->IsTrigger = true;

	Physics->RemoveCollisionObject(m_cursor.Find<iw::CollisionObject>());

	return Layer::Initialize();
}

void GameLayer::Update() 
{ // this is one frame behind, add a callback to the sand layer that gets called at the right time, right after rendering the world...
	
	m_cursor.Find<iw::CollisionObject>()->Transform.Position = glm::vec3(sand->sP, 0.f);

	// state change :o

	//if (showGameOver)
	//{
	//	if (iw::Keyboard::KeyDown(iw::InputName::SPACE))
	//	{
	//		// could reset by popping all layers and pushing them again
	//		// each systems OnPush / OnPop could clean up and reint

	//		// now doing it with events END_GAME and RUN_GAME
	//		// i think that the push/pop was is better but this is simpler
	//		// ill have to design it better after this weekend when there are actual menus
	//		// there will have to be more state, which will require the systems being push around
	//		// maybe layers for menus too actually

	//		Bus->push<StateChange_Event>(RUN_STATE);
	//	}

	//	return;
	//}

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
bool GameLayer::On(iw::ActionEvent& e)
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
		case STATE_CHANGE:
		{
			StateChange_Event& event = e.as<StateChange_Event>();

			switch (event.State)
			{
				case END_STATE:
				{
					showGameOver = true;
					PopSystem(playerTank_s);
					PopSystem(world_s);
					PopSystem(keepInWorld_s);

					break;
				}
				case RUN_STATE: 
				{
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
				case PAUSE_STATE:
				{
					uiHideBar = true;
					uiExpandBackground = true;

					PopSystem(player_s);
					PopSystem(playerTank_s);
					PopSystem(world_s);
					PopSystem(projectile_s);
					PopSystem(enemy_s);
					PopSystem(item_s);
					PopSystem(keepInWorld_s);
					PopSystem(score_s);
					PopSystem(corePixels_s);
					PopSystem(flocking_s);
					PopSystem(explosion_s);

					break;
				}
				case RESUME_STATE:
				{
					uiHideBar = false;
					uiExpandBackground = false;

					PushSystem(player_s);
					PushSystem(playerTank_s);
					PushSystem(world_s);
					PushSystem(projectile_s);
					PushSystem(enemy_s);
					PushSystem(item_s);
					PushSystem(keepInWorld_s);
					PushSystem(score_s);
					PushSystem(corePixels_s);
					PushSystem(flocking_s);
					PushSystem(explosion_s);

					break;
				}
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

void GameLayer::PostUpdate()
{
	sand->m_drawMouseGrid = iw::Keyboard::KeyDown(iw::SHIFT);

	if (m_player.Alive()) 
	{
		int ammo = m_player.Find<Player>()->CurrentWeapon->Ammo;

		if (ammo != CachedAmmo)
		{
			CachedAmmo = ammo;
			A_font_cambria->UpdateMesh(A_mesh_ui_text_ammo, tos(CachedAmmo), 5);
		}
	}

	A_font_cambria->UpdateMesh(A_mesh_ui_text_ammo, tos(iw::DeltaTime() * 1000), 5);

	if (score_s->Score != CachedScore)
	{
		CachedScore = score_s->Score;
		A_font_cambria->UpdateMesh(A_mesh_ui_text_score, tos(CachedScore), 5);
	}

	A_font_cambria->UpdateMesh(A_mesh_ui_text_score, tos(iw::DeltaTime()*1000), 5);

	float red;

	if (showGameOver)
	{
		red = 0;
		uiJitterAmount = 10;
	}

	else
	{
		CorePixels* core = m_player.Find<CorePixels>();
		float death = core->Timer / core->TimeWithoutCore;
		red = 1 - death; //(cos(iw::TotalTime() * 1.5f * death) + 1) / 2;

		uiJitterAmount = 75 * death;
		uiJitterAmount = iw::lerp(uiJitterAmount, 0.f, iw::DeltaTime() * 10);
	}

	A_mesh_ui_background  .Material->Set("color", iw::Color(1, red, red));
	A_mesh_ui_playerHealth.Material->Set("color", iw::Color(1, red, red));

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

	float uiBarOffsetTarget = 0;
	if (uiHideBar) uiBarOffsetTarget = -menu->height;
	else           uiBarOffsetTarget =  menu->height;
	uiBarOffset = iw::lerp(uiBarOffset, uiBarOffsetTarget, iw::DeltaTime() * 12);

	menu->x = iw::randf() * uiJitterAmount;                                // Random(uiJitterAmount)
	menu->y = iw::randf() * uiJitterAmount - screen.height + uiBarOffset; //Combine(Anchor(BOTTOM), menu->height, Random(uiJitterAmount))

	game->y = menu->height;                      // Same(manu->height)
	game->height = screen.height - menu->height; // fill rest of screen y
	game->width  = game->height;                 // Same(game->height)

	menu->width = game->width; // Same(game->width)

	float menu_pad = menu->height * .2f;

	UI* health = screen.CreateElement(A_mesh_ui_playerHealth);
	UI* laser = screen.CreateElement(sand_ui_laserCharge->GetSandMesh());

	health->z = 2; 
	laser->z = -2; // ?? why does this have to be negitive

	health->height = menu->height - menu_pad;
	health->width = health->height; // Ratio(health->height)
	health->x = menu->x;            // Same(menu->x)
	health->y = menu->y;            // Same(menu->y)

	laser->height = menu->height - menu_pad;
	laser->width = laser->height;                               // Same (laser->height)
	laser->x = menu->x - menu->width + laser->width + menu_pad; // Combine(Anchor(LEFT), laser->width, menu_pads)
	laser->y = menu->y;

	float menu_pixel_x_scale = menu->width  / A_texture_ui_background->m_width;
	float menu_pixel_y_scale = menu->height / A_texture_ui_background->m_height;

	float ammo_x_pad  = (floor(log10(CachedAmmo))  * 11 + 30) * menu_pixel_x_scale;
	float score_x_pad = (floor(log10(CachedScore)) * 11 + 30) * menu_pixel_x_scale;

	float ammo_y_pad = 13 * menu_pixel_y_scale;
	float score_y_pad = 48 * menu_pixel_y_scale;

	UI* ammo   = screen.CreateElement(A_mesh_ui_text_ammo);
	UI* score = screen.CreateElement(A_mesh_ui_text_score);

	ammo->width  = menu->height;
	ammo->height = menu->height; // Same(score->width)
	ammo->x = menu->x + menu->width - ammo_x_pad;
	ammo->y = menu->y + menu->height - ammo_y_pad;
	ammo->z = 2;

	score->width  = menu->height;
	score->height = menu->height; // Same(score->width)
	score->x = menu->x + menu->width - score_x_pad;
	score->y = menu->y + menu->height - score_y_pad;
	score->z = 2;

	if (showGameOver)
	{
		UI* gameover = screen.CreateElement(A_mesh_ui_text_gameOver);
		gameover->x = -screen.width*.8;
		gameover->y = screen.height * .5;
		gameover->width = screen.width;
		gameover->height = screen.width;
		gameover->z = 5;
	}

	float uiBackgroundScaleTarget = 0;
	if (uiExpandBackground) uiBackgroundScaleTarget = 3;
	else                    uiBackgroundScaleTarget = 1;
	uiBackgroundScale = iw::lerp(uiBackgroundScale, uiBackgroundScaleTarget, iw::DeltaTime() * 12);

	UI* background = screen.CreateElement(A_mesh_background);
	background->y = game->y;
	background->width  = game->width  * uiBackgroundScale;
	background->height = game->height * uiBackgroundScale;
	background->z = -1;

	UI* version = screen.CreateElement(A_mesh_ui_text_debug_version);
	version->y = screen.height - 12;
	version->x = -screen.width + 1;
	version->width  = menu->width;
	version->height = menu->width;
	version->z = 5;

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

// APP //

App::App() : iw::Application() 
{
	state = RUN_STATE;
	menu = nullptr;

	int cellSize  = 2;
	int cellMeter = 1;
	bool drawWithMouse = false;
	
	sand = new iw::SandLayer(cellSize, cellMeter, 800, 800, 4, 4, drawWithMouse);
	sand->m_updateDelay = 1/144.f;

	sand_ui_laser = new iw::SandLayer(1, 1, 40, 40);
	sand_ui_laser->m_updateDelay = .016f;

	game = new GameLayer(sand, sand_ui_laser);

	PushLayer(sand);
	PushLayer(sand_ui_laser);
	PushLayer(game);
}

int App::Initialize(
	iw::InitOptions& options)
{
	iw::ref<iw::Context> context = Input->CreateContext("Game");
		
	context->MapButton(iw::SPACE , "action");
	context->MapButton(iw::D     , "+right");
	context->MapButton(iw::A     , "-right");
	context->MapButton(iw::W     , "+forward");
	context->MapButton(iw::S     , "-forward");
	context->MapButton(iw::ESCAPE, "esc-menu");

	context->AddDevice(Input->CreateDevice<iw::Mouse>());
	context->AddDevice(Input->CreateDevice<iw::RawKeyboard>());
	context->AddDevice(Input->CreateDevice<iw::RawMouse>());

	Console->AddHandler([&](
		const iw::Command& command)
	{
		if (command.Verb == "esc-menu" && !game->showGameOver)
		{
			switch (state)
			{
				case RUN_STATE:
				{
					PopLayer(sand);
					PopLayer(sand_ui_laser);

					if (!menu) menu = new MenuLayer();
					PushLayer(menu);

					state = PAUSE_STATE;
					Bus->push<StateChange_Event>(PAUSE_STATE);

					Physics->Paused = true;

					break;
				}
				case PAUSE_STATE:
				{
					PushLayer(sand);
					PushLayer(sand_ui_laser);

					if (menu) PopLayer(menu);

					state = RUN_STATE;
					Bus->push<StateChange_Event>(RESUME_STATE);

					Physics->Paused = false;

					break;
				}
			}
		}

		else if (command.Verb == "action" && game->showGameOver)
		{
			Bus->push<StateChange_Event>(RUN_STATE);
		}

		return false;
	});

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
		true,/*false,*/
		iw::DisplayState::NORMAL
	};

	return new App();
}

iw::Application* GetApplicationForEditor() {
	return new App();
}

void MenuLayer::PostUpdate()
{
	UIScreen screen(Renderer->Width(), Renderer->Height(), 10);

	UI* background = screen.CreateElement(A_mesh_menu_background);
	background->width  = screen.width;
	background->height = screen.height;
	background->z = -5;

	UI* pause_menu = screen.CreateElement(A_mesh_menu_pause);
	pause_menu->height = 600;
	pause_menu->width  = 600;
	pause_menu->z = -6;

	float pause_title_margin_x = 15;
	float pause_title_margin_y = -5;

	UI* pause_title = screen.CreateElement(A_mesh_menu_pause_title);
	pause_title->width  = pause_menu->width;
	pause_title->height = pause_menu->width;
	pause_title->x = pause_menu->x - pause_menu->width  + pause_title_margin_x;
	pause_title->y = pause_menu->y + pause_menu->height + pause_title_margin_y;
	pause_title->z = -7;

	screen.Draw(nullptr, Renderer);
}
