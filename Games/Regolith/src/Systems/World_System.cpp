#include "Systems/World_System.h"
#include "iw/engine/Events/Seq/Delay.h"

void WorldSystem::Update()
{
	m_timer.Tick();

	bool needsAnotherLevel = false;

	if (m_levels.size() > 0)
	{
		if (m_levels.back().Update())
		{
			m_levels.pop_back();
			needsAnotherLevel = true;
		}
	}

	else {
		needsAnotherLevel = true;
	}

	if (needsAnotherLevel)
	{
		int enemyCount = 0;
		Space->Query<Enemy>().Each([&](iw::EntityHandle handle, Enemy*) {
			enemyCount++;
		});

		iw::EventSequence& seq = m_levels.emplace_front(CreateSequence());
		if (enemyCount < 15)
		{
			seq.Add<Spawn>(MakeEnemySpawner())
			   .And<Spawn>(MakeAsteroidSpawner())
			   .And<iw::Delay>(10);
		}

		else {
			seq.Add<Spawn>(MakeAsteroidSpawner());
		}

		seq.Start();
	}
}

void WorldSystem::FixedUpdate()
{
	Space->Query<iw::Transform, iw::Tile, Asteroid>().Each(
		[&](
			iw::EntityHandle handle, 
			iw::Transform* transform,
			iw::Tile* tile,
			Asteroid* asteroid) 
		{
			asteroid->Lifetime += iw::FixedTime();

			iw::AABB2 b = iw::TransformBounds(tile->m_bounds, transform);

			if (   asteroid->Lifetime > 10
				&& !iw::AABB2(glm::vec2(200.f), 200).Intersects(&iw::Transform(), tile->m_bounds, transform))
			{
				Space->QueueEntity(handle, iw::func_Destroy);
			}
		});

	//Space->Query<Throwable, Flocker>().Each(
	//	[&](
	//		iw::EntityHandle handle, 
	//		Throwable* throwable, 
	//		Flocker* flocker) 
	//	{
	//		flocker->Active = !throwable->Held;
	//	});

	Space->Query<iw::Rigidbody, Throwable>().Each(
		[&](
			iw::EntityHandle handle, 
			iw::Rigidbody* rigidbody,
			Throwable* throwable) 
		{
			if (throwable->Held)
			{
				if (!throwable->ThrowRequestor.Alive())
				{
					return;
				}

				throwable->Timer += iw::FixedTime();

				float distFromThrower = glm::distance(
					rigidbody->Transform.Position, 
					throwable->ThrowRequestor.Find<iw::Transform>()->Position);

				if (   throwable->Timer > throwable->Time
					|| distFromThrower  > 150)
				{
					throwable->Held = false;
				}

				glm::vec3 pos  = rigidbody->Transform.Position;
				glm::vec3 tpos = throwable->ThrowTarget   .Find<iw::Transform>()->Position;
				glm::vec3 rpos = throwable->ThrowRequestor.Find<iw::Transform>()->Position;

				/*
				auto [min, max] = rigidbody->Collider->as<iw::Physics::impl::Collider<iw::d2>>()->Bounds();
				float radius = glm::length(max - min) / 2.f;
				*/

				for (int i = 0; i < 4; i++)
				{
					//float a = iw::Pi2 * iw::randf();

					float rx = 0.f;//sin(a) * 20.f;
					float ry = 0.f;//cos(a) * 20.f;

					DrawLightning(
						pos.x  + rx, pos.y  + ry, 
						rpos.x + ry, rpos.y + rx);
				}

				glm::vec3 vel = tpos - pos;
				vel = glm::normalize(vel);

				float currentSpeed = glm::length(rigidbody->Velocity);
				float speed = iw::max(currentSpeed, 100.f * throwable->Timer / throwable->Time);

				rigidbody->Velocity = iw::lerp(rigidbody->Velocity, vel * speed, iw::FixedTime() * 25);
			}
		});
}

bool WorldSystem::On(iw::ActionEvent& e)
{
	switch (e.Action)
	{
		case PROJ_HIT_TILE:
		{
			auto& [x, y, info, hit, projectile] = e.as<ProjHitTile_Event>().Config;
			
			if (   hit.Has<Player>() 
				|| hit.Has<Enemy>())
			{
				glm::vec3 norm = glm::normalize(projectile.Find<iw::Rigidbody>()->Velocity);

				iw::Cell metal;
				metal.Type = iw::CellType::ROCK;
				metal.Props = iw::CellProp::MOVE_FORCE;
				
				metal.Color = iw::Color::From255(255, 207, 77);
				
				metal.dx = norm.x * 300 + iw::randfs() * 100;
				metal.dy = norm.y * 300 + iw::randfs() * 100;
				metal.life = .05 + iw::randf() * .2;

				float margin = 15;
				float px = x + norm.x * margin;
				float py = y + norm.y * margin;
				
				sand->m_world->SetCell(px, py, metal);
			}

			// todo: if entity is an asteroid
			// eject dirt or something, gravitized to nearest asteroid

			break;
		}
		case REMOVE_CELL_FROM_TILE:
		{
			RemoveCellFromTile_Event& event = e.as<RemoveCellFromTile_Event>();
			sand->EjectPixel(event.Entity.Find<iw::Tile>(), event.Index);

			break;
		}
		case CORE_EXPLODED:
		{
			iw::Entity& entity = e.as<CoreExploded_Event>().Entity;

			glm::vec3 pos = entity.Find<iw::Transform>()->Position;

			SpawnExplosion_Config config;
			config.SpawnLocationX = pos.x;
			config.SpawnLocationY = pos.y;
			config.ExplosionRadius = 20;

			if (entity.Has<Enemy>())
			{
				config.ExplosionPower = entity.Find<Enemy>()->ExplosionPower;
			}

			else 
			{
				CorePixels* core = entity.Find<CorePixels>();
				for (int i : core->ActiveIndices)
				{
					Bus->push<RemoveCellFromTile_Event>(i, entity);
				}

				config.ExplosionPower = 50; // player explosion
			}

			Bus->push<SpawnExplosion_Event>(config);

			break;
		}
		case SPAWN_ASTEROID: {
			MakeAsteroid(e.as<SpawnAsteroid_Event>().Config);
			break;
		}
		case CREATED_PLAYER: {
			m_player = e.as<CreatedPlayer_Event>().PlayerEntity;

			//SpawnAsteroid_Config c;
			//c.SpawnLocationX = 100;
			//c.SpawnLocationY = 100;
			//c.Size = 1;

			//Bus->push<SpawnAsteroid_Event>(c);

			//SpawnEnemy_Config cc;
			//cc.SpawnLocationX = 300;
			//cc.SpawnLocationY = 100;
			//cc.TargetLocationX = 200;
			//cc.TargetLocationY = 200;
			//cc.EnemyType = BASE;
			//cc.TargetEntity = m_player;

			//Bus->push<SpawnEnemy_Event>(cc);

			break;
		}
		case STATE_CHANGE:
		{
			StateChange_Event& event = e.as<StateChange_Event>();

			switch (event.State)
			{
				case GAME_START_STATE:
				{
					m_levels.emplace_front(CreateSequence())
						.Add<Spawn>(MakeEnemySpawner())
						.And<Spawn>(MakeAsteroidSpawner())
						.And<iw::Delay>(30)
						.Start();
					break;
				}
				case GAME_OVER_STATE:
				{
					m_levels.clear();
					break;
				}
			}

			break;
		}
	}

	return false;
}

void WorldSystem::DrawLightning(
	float x, float y, 
	float tx, float ty)
{
	float dx = tx - x;
	float dy = ty - y;
	float td = sqrt(dx*dx + dy*dy);
	float  d = td;

	float arc = 10;
	float stepSize = 5;

	LOG_INFO << d;
	while (d > 10)
	{
		dx = tx - x;
		dy = ty - y;
      
		d = iw::max(1.f, sqrt(dx*dx + dy*dy));

		dx = dx / d * stepSize;
		dy = dy / d * stepSize;
      
		float left = d / td;
      
		if (left > 1)
		{
			left = .1;
		}

		float x1 = x + dx + left * iw::randfs() * arc;
		float y1 = y + dy + left * iw::randfs() * arc;
      
		//stroke(255 * left, 255 * left, 255);
      
		iw::Cell c = iw::Cell::GetDefault(iw::CellType::ROCK);
		c.life = .04;

		c.Color = iw::Color::From255(212, 194, 252);
		c.StyleColor = iw::Color(.1);
		c.StyleOffset = iw::randfs();
		c.Style = iw::CellStyle::SHIMMER;

		sand->ForEachInLine(x, y, x1, y1,
			[&](
				float x, float y)
			{
				sand->m_world->SetCell(x, y, c);
				return false;
			});
      
		x = x1;
		y = y1;
	}
}

iw::Entity WorldSystem::MakeAsteroid(
	SpawnAsteroid_Config& config)
{
	iw::ref<iw::Texture> asteroid_tex;

	switch (config.Size)
	{
		case 0: asteroid_tex = A_texture_asteroid_mid_1; break;
		case 1: asteroid_tex = A_texture_asteroid_mid_2; break;
		case 2: asteroid_tex = A_texture_asteroid_mid_3; break;
	}

	iw::Entity entity = sand->MakeTile<iw::MeshCollider2, Asteroid, Throwable>(asteroid_tex, true);
	iw::Transform* t = entity.Find<iw::Transform>();
	iw::Rigidbody* r = entity.Find<iw::Rigidbody>();

	t->Position = glm::vec3(config.SpawnLocationX, config.SpawnLocationY, 0);

	r->Velocity = glm::vec3(config.VelocityX, config.VelocityY, 0);
	r->AngularVelocity.z = config.AngularVel;
	r->SetTransform(t);
	r->SetMass(1000);

	return entity;
}

//void WorldSystem::SetupLevels()
//{
//	m_runningLevels.clear();
//	m_levels.clear();
//
//	// should load from files
//
//	auto [w, h] = sand->GetSandTexSize();
//	sand->SetCamera(w / 2, h / 2);
//
//	iw::EventSequence& level1 = m_levels.emplace_back(CreateSequence());
//
//	Spawn spawner(10, 2, .5, .3);
//	spawner.AddSpawn(w / 2 - 50, h + 10, w / 2 + 50, h + 100);
//	spawner.OnSpawn = SpawnEnemy;
//
//	level1.Add([&]()
//		{
//			SpawnAsteroid_Config config;
//			SpawnAsteroid_Config config1;
//		
//			config.SpawnLocationX = 0;
//			config.SpawnLocationY = 550;
//			config.VelocityX =  20;
//			config.VelocityY = -100;
//			config1.AngularVel = -iw::randfs() / 3;
//
//			config1.SpawnLocationX = 400;
//			config1.SpawnLocationY = 550;
//			config1.VelocityX = -20;
//			config1.VelocityY = -100;
//			config1.AngularVel = iw::randfs() / 3;
//
//			Bus->push<SpawnAsteroid_Event>(config);
//			Bus->push<SpawnAsteroid_Event>(config1);
//
//			return true;
//		})
//		->Add<iw::Delay>(15)
//		->Add<Spawn>(spawner)
//		->Add([&]() {
//			m_levels.pop_back();
//			return true;
//		});
//
//	m_levels.back().Start();
//}

//Spawn WorldSystem::MakeSpawner(
//	int numb, int batch,
//	float time, float timeMargin,
//	std::initializer_list<WhereToSpawn> where)
//{
//	Spawn spawn(numb, batch, time, timeMargin);
//	
//	auto [w, h] = sand->GetSandTexSize();
//
//	for (WhereToSpawn side : where)
//	{
//		switch (side)
//		{
//			case TOP:    spawn.AddSpawn(  - 100, h + 100, w + 100, h + 200); break;
//			case BOTTOM: spawn.AddSpawn(  - 100,   - 100, w + 100,   - 200); break;
//			case LEFT:   spawn.AddSpawn(w - 100,   - 100, w - 200, h + 200); break;
//			case RIGHT:  spawn.AddSpawn(w + 100,   - 100, w + 200, h + 200); break;
//		}
//	}
//
//	return spawn;
//}
//
//Spawn WorldSystem::MakeEnemySpawnner(
//	int numb, int batch,
//	float time, float timeMargin)
//{
//	Spawn spawn = MakeSpawner(numb, batch, time, timeMargin, where);
//	spawn.func_Spawn = [=](float x, float y)
//	{
//		auto [w, h] = sand->GetSandTexSize();
//		float margin = .1f;
//
//		float target_x = iw::randi(w - w * margin * 2) + w * margin;
//		float target_y = iw::randi(h - h * margin * 2) + h * margin;
//
//		Bus->push<SpawnEnemy_Event>(m_player, x, y, target_x, target_y);
//	};
//	
//	return spawn;
//}
//
//Spawn WorldSystem::MakeAsteroidSpawnner(
//	std::initializer_list<WhereToSpawn> where)
//{
//	Spawn spawn = MakeSpawner(where, 3, 1, 2, 1);
//	spawn.func_Spawn = [=](float x, float y)
//	{
//		SpawnAsteroid_Config config;
//		config.SpawnLocationX = x;
//		config.SpawnLocationY = y;
//		config.VelocityX = iw::randfs() * 10;
//		config.VelocityY = -20 - iw::randfs() * 10;
//		config.AngularVel = iw::randfs() / 10;
//		config.Size = iw::randi(10);
//
//		Bus->push<SpawnAsteroid_Event>(config);
//	};
//
//	return spawn;
//}

//Spawn WorldSystem::MakeAsteroidSpawnner()
//{
//	return Spawn();
//}
