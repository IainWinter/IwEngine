#include "Systems/World_System.h"

int WorldSystem::Initialize()
{
	auto [w, h] = sand->GetSandTexSize();
	sand->SetCamera(w / 2, h / 2);

	Spawn spawn(20, 4, 5.f, 3.f, [=](float x, float y)
	{
		auto [w, h] = sand->GetSandTexSize();
		float margin = .1f;

		float target_x = iw::randi(w - w * margin * 2) + w * margin;
		float target_y = iw::randi(h - h * margin * 2) + h * margin;

		Bus->push<SpawnEnemy_Event>(m_player, x, y, target_x, target_y);
	});
	spawn.AddSpawn(0, h + 10, w, h + 20);
	spawn.AddSpawn(0,   - 10, w,   - 20);

	//Fill fillTicTacToe([=](int x, int y) 
	//{
	//	sand->m_world->SetCell(x, y, iw::Cell::GetDefault(iw::CellType::ROCK));
	//});
	//fillTicTacToe.AddFill(0, h - 100, w, h - 120, Fill::LEFT_RIGHT);
	//fillTicTacToe.AddFill(0,     100, w,     120, Fill::LEFT_RIGHT);
	//fillTicTacToe.AddFill(w - 100, 0, w - 120, h);
	//fillTicTacToe.AddFill(    100, 0,     120, h);

	//Fill fillBoarder([=](int x, int y) 
	//{
	//	sand->m_world->SetCell(x, y, iw::Cell::GetDefault(iw::CellType::ROCK));
	//});
	//fillBoarder.AddFill(0, h - 10,  w,  h, Fill::LEFT_RIGHT);
	//fillBoarder.AddFill(0,      0,  w, 10, Fill::LEFT_RIGHT);
	//fillBoarder.AddFill(w - 10, 0,  w,  h);
	//fillBoarder.AddFill(0,      0, 10,  h);

	iw::EventSequence& level1 = m_levels.emplace_back(CreateSequence());

	//level1.Add<Fill>(fillBoarder);
	level1.Add<Spawn>(spawn);

	level1.Add([&]() {
		m_levels.pop_back();
		return true;
	});

	m_levels.back().Start();

	for (int i = 0; i < 5; i++)
	{
		iw::Entity asteroid = sand->MakeTile(A_texture_asteroid, true);
		iw::Transform* tran = asteroid.Find<iw::Transform>();
		iw::Rigidbody* body = asteroid.Find<iw::Rigidbody>();

		tran->Position = glm::vec3(iw::randf() * 600 - 100, iw::randf() * 400 + 300, 0);
		body->SetTransform(tran);
		body->Velocity = glm::vec3(iw::randfs() * 10, -iw::randf() * 10 - 5, 0);
		body->AngularVelocity.z = iw::randfs() / 10;
		body->SetMass(1000000);
	}

	return 0;
}

void WorldSystem::Update()
{
	if (m_levels.size() > 0)
	{
		m_levels.back().Update();
	}
}

bool once = true;
int x = 0;

void WorldSystem::FixedUpdate()
{
	Space->Query<iw::Transform, iw::Tile>().Each(
		[&](
			iw::EntityHandle entity, 
			iw::Transform* transform,
			iw::Tile* tile) 
		{
			if (tile->m_currentCellCount < tile->m_initalCellCount / 3)
			{
				if (    Space->HasComponent<Player>   (entity) 
					|| !Space->HasComponent<EnemyShip>(entity))
				{
					return;
				}

				std::vector<std::pair<ItemType, float>> item_weights{
					//{ HEALTH, 50},
					//{ LASER_CHARGE,  50 },
					{ WEAPON_MINIGUN, 5 }
				};

				ItemType item = iw::choose(item_weights);
				int amount = 0;

				switch (item)
				{
					case ItemType::HEALTH: 
					case ItemType::LASER_CHARGE:   amount = iw::randi(5) + 1; break;

					case ItemType::WEAPON_MINIGUN: amount = 1;
				}

				Bus->push<SpawnItem_Event>(transform->Position.x, transform->Position.y, amount, item);
				
				iw::Cell smoke = iw::Cell::GetDefault(iw::CellType::SMOKE);

				for (int y = -5; y < 5; y++)
				for (int x = -5; x < 5; x++)
				{
					int px = x + transform->Position.x;
					int py = y + transform->Position.y;
					smoke.life = iw::randf() * 5;

					sand->m_world->SetCell(px, py, smoke);
				}

				Space->QueueEntity(entity, iw::func_Destroy);
			}
		});

	// DEBUG

	if (iw::Mouse::ButtonDown(iw::MMOUSE))
	{
		if (iw::Keyboard::KeyDown(iw::SHIFT))
		{
			Bus->push<SpawnItem_Event>(sand->sP.x, sand->sP.y, 1, ItemType::LASER_CHARGE);
		}

		else {
			Bus->push<SpawnItem_Event>(sand->sP.x, sand->sP.y, 1, ItemType::HEALTH);
		}
	}

	if (iw::Keyboard::KeyDown(iw::X))
	{
		for (int i = 0; i < 5; i++) {
			for (int y = 0; y < 800; y++)
			{
				sand->m_world->SetCell(x, y, iw::Cell::GetDefault(iw::CellType::ROCK));
			}
			x++;
		}
	}
}

bool WorldSystem::On(iw::ActionEvent& e)
{
	switch (e.Action)
	{
		case PROJ_HIT_TILE:
		{
			ProjHitTile_Event& event = e.as<ProjHitTile_Event>();

			sand->EjectPixel(event.Info.tile, event.Info.index);
			event.Hit.Find<iw::Rigidbody>()->ApplyForce(event.Projectile.Find<iw::Rigidbody>()->Velocity);

			break;
		}
	}

	return false;
}
