#include "Systems/World_System.h"

int WorldSystem::Initialize()
{
	auto [w, h] = sand->GetSandTexSize();
	sand->SetCamera(w / 2, h / 2);

	iw::EventSequence& level1 = m_levels.emplace_back(CreateSequence());

	level1./*Add<Spawn>(MakeEnemySpawnner   ({ TOP }))
		 ->*/Add<Spawn>(MakeAsteroidSpawnner({ TOP }));

	level1.Add([&]() {
		m_levels.pop_back();
		return true;
	});

	m_levels.back().Start();

	return 0;
}

void WorldSystem::Update()
{
	if (m_levels.size() > 0)
	{
		m_levels.back().Update();
	}
}

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
}

bool WorldSystem::On(iw::ActionEvent& e)
{
	switch (e.Action)
	{
		case PROJ_HIT_TILE:
		{
			ProjHitTile_Event& event = e.as<ProjHitTile_Event>();
			sand->EjectPixel(event.Info.tile, event.Info.index);
			break;
		}
		case SPAWN_ASTEROID: {
			MakeAsteroid(e.as<SpawnAsteroid_Event>().Config);
			break;
		}
	}

	return false;
}

iw::Entity WorldSystem::MakeAsteroid(
	SpawnAsteroid_Config& config)
{
	iw::Entity entity = sand->MakeTile(A_texture_asteroid, true);
	iw::Transform* t = entity.Find<iw::Transform>();
	iw::Rigidbody* r = entity.Find<iw::Rigidbody>();

	t->Position = glm::vec3(config.SpawnLocationX, config.SpawnLocationY, 0);
	r->Velocity = glm::vec3(config.VelocityX, config.VelocityY, 0);
	r->AngularVelocity.z = config.AngularVel;
	
	r->SetTransform(t);
	r->SetMass(1000000);

	return entity;
}

Spawn WorldSystem::MakeSpawner(
	std::initializer_list<WhereToSpawn> where,
	int numb, int batch,
	float time, float timeMargin)
{
	Spawn spawn(numb, batch, time, timeMargin);
	
	auto [w, h] = sand->GetSandTexSize();

	for (WhereToSpawn side : where)
	{
		switch (side)
		{
			case TOP:    spawn.AddSpawn(  - 100, h + 100, w + 100, h + 200); break;
			case BOTTOM: spawn.AddSpawn(  - 100,   - 100, w + 100,   - 200); break;
			case LEFT:   spawn.AddSpawn(w - 100,   - 100, w - 200, h + 200); break;
			case RIGHT:  spawn.AddSpawn(w + 100,   - 100, w + 200, h + 200); break;
		}
	}

	return spawn;
}

Spawn WorldSystem::MakeEnemySpawnner(
	std::initializer_list<WhereToSpawn> where)
{
	Spawn spawn = MakeSpawner(where, 20, 4, 5, 3);
	spawn.func_Spawn = [=](float x, float y)
	{
		auto [w, h] = sand->GetSandTexSize();
		float margin = .1f;

		float target_x = iw::randi(w - w * margin * 2) + w * margin;
		float target_y = iw::randi(h - h * margin * 2) + h * margin;

		Bus->push<SpawnEnemy_Event>(m_player, x, y, target_x, target_y);
	};
	
	return spawn;
}

Spawn WorldSystem::MakeAsteroidSpawnner(
	std::initializer_list<WhereToSpawn> where)
{
	Spawn spawn = MakeSpawner(where, 3, 1, 2, 1);
	spawn.func_Spawn = [=](float x, float y)
	{
		SpawnAsteroid_Config config;
		config.SpawnLocationX = x;
		config.SpawnLocationY = y;
		config.VelocityX = iw::randfs() * 10;
		config.VelocityY = -20 - iw::randfs() * 10;
		config.AngularVel = iw::randfs() / 10;
		config.Size = iw::randi(10);

		Bus->push<SpawnAsteroid_Event>(config);
	};

	return spawn;
}

//Spawn WorldSystem::MakeAsteroidSpawnner()
//{
//	return Spawn();
//}
