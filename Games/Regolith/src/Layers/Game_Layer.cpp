#include "Layers/Game_Layer.h"

int Game_Layer::Initialize()
{
	auto [sandWidth, sandHeight] = sand->GetSandTexSize2();
	sand->SetCamera(sandWidth, sandHeight);

	Renderer->Device->SetClearColor(0, 0, 0, 1);

	Physics->AddSolver(new iw::SmoothPositionSolver());
	Physics->AddSolver(new iw::ImpulseSolver());

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

	PushSystem(playerTank_s);
	PushSystem(world_s);
	PushSystem(projectile_s);
	PushSystem(enemy_s);
	PushSystem(player_s);
	PushSystem(score_s);
	PushSystem(corePixels_s);
	PushSystem(flocking_s);
	PushSystem(item_s);
	PushSystem(explosion_s);
	PushSystem(keepInWorld_s);

	//Bus->push<StateChange_Event>(RUN_STATE);

	//m_cursor = sand->MakeTile(A_texture_ui_cursor);
	//m_cursor.Find<iw::Tile>()->m_zIndex = 1;
	//m_cursor.Find<iw::CollisionObject>()->IsTrigger = true;

	//Physics->RemoveCollisionObject(m_cursor.Find<iw::CollisionObject>());

	return Layer::Initialize();
}

void Game_Layer::Destroy()
{
	iw::ComponentQuery query = Space->MakeQuery();

	query.SetAny({
		Space->GetComponent<Player>(),
		Space->GetComponent<Enemy>(),
		Space->GetComponent<Asteroid>(),
		Space->GetComponent<ProjHead>(),
		Space->GetComponent<Item>()
	});

	Space->Query(query).Each([&](iw::EntityHandle handle) {
		Space->QueueEntity(handle, iw::func_Destroy);
	});

	Layer::Destroy();
}

void Game_Layer::Update() 
{
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

	//if (iw::Mouse::ButtonDown(iw::RMOUSE))
	//{
	//	//SpawnItem_Config config;
	//	//config.X = sand->sP.x;
	//	//config.Y = sand->sP.y;
	//	//config.Item = ItemType::HEALTH;

	//	//Bus->push<SpawnItem_Event>(config);

	//	Console->QueueCommand("game-over");
	//}
}

bool Game_Layer::On(iw::ActionEvent& e)
{
	switch(e.Action) 
	{
		case STATE_CHANGE:
		{
			StateChange_Event& event = e.as<StateChange_Event>();

			switch (event.State)
			{
				case GAME_OVER_STATE:
				{
					DestroySystem(keepInWorld_s);
					DestroySystem(playerTank_s);
					DestroySystem(world_s);
					DestroySystem(player_s);
					DestroySystem(enemy_s);    

					// needed for event queue of state change before 
					// spawning item for player death spawning core peices
					//PopSystem(item_s); 

					//std::vector<iw::event_record*> events = Bus->end_record();

					/*for (iw::event_record* record : events)
					{
						LOG_DEBUG << "[Event] " << record->type->name;
					}*/

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

void Game_Layer::PostUpdate()
{
	//m_cursor.Find<iw::CollisionObject>()->Transform.Position = glm::vec3(sand->sP, 0.f);

	sand->m_drawMouseGrid = iw::Keyboard::KeyDown(iw::SHIFT);

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
