#include "Systems/Enemy_System.h"

void EnemySystem::FixedUpdate()
{
	for (auto [rigidbody, ship] : entities().query<iw::Rigidbody, Enemy>())
	{
		if (rigidbody.IsTrigger)
		{
			ship.ActiveTimer += iw::FixedTime();
		
			if (ship.ActiveTimer > ship.ActiveTime)
			{
				rigidbody.IsTrigger = false;
			}
		}
	}

	for (auto [entity, transform, ship, fighter] : entities().query<iw::Transform, Enemy, Fighter_Enemy>().with_entity())
	{
		if (   !ship.Target.is_alive()
			|| !fighter.Weapon->CanFire()
			|| abs(transform.Position.x - 200) > 200
			|| abs(transform.Position.y - 200) > 200)
		{
			continue;
		}

		glm::vec3 pos = ship.Target.get<iw::Transform>().Position;
		ShotInfo shot = fighter.Weapon->GetShot(entity, pos.x, pos.y);
		Bus->push<SpawnProjectile_Event>(shot);
		Bus->push<PlaySound_Event>(fighter.Weapon->Audio);
	}

	for (auto [entity, transform, tile, flocker, ship, bomb] : entities().query<iw::Transform, iw::Tile, Flocker, Enemy, Bomb_Enemy>().with_entity())
	{
		// maybe put some timer or something
		
		if (!ship.Target.is_alive())
		{
			continue;
		}
		
		glm::vec3 pos = ship.Target.get<iw::Transform>().Position;
			
		// set flocking target to player pos
		flocker.Target = glm::vec2(pos.x, pos.y);
		
		// start explosion if close to target
		if (   !bomb.Explode 
			&& glm::distance(pos, transform.Position) < bomb.RadiusToExplode)
		{
			bomb.Explode = true;
			entities_defer().remove<Flocker>(entity);
		}
		
		if (bomb.Explode)
		{
			bomb.TimeToExplode -= iw::FixedTime();
				
			tile.m_tint = iw::Color(1, .1, .1);
		
			if (bomb.TimeToExplode <= 0.f)
			{
				SpawnExplosion_Config config;
				config.SpawnLocationX = transform.Position.x;
				config.SpawnLocationY = transform.Position.y;
				config.ExplosionPower = 900;
				config.ExplosionRadius = 10;
		
				Bus->push<SpawnExplosion_Event>(config);
				//entities_defer().destroy(entity);
			}
		}
	}

	for (auto [entity, transform, ship, station] : entities().query<iw::Transform, Enemy, Station_Enemy>().with_entity())
	{
		if (!ship.Target.is_alive())
		{
			continue;
		}

		station.Timer.TickFixed();
		if (station.Timer.Can("spawn"))
		{
			SpawnEnemy_Config config;

			glm::vec3 right = transform.Right();
			switch (iw::randi(2))
			{
				case 0: right = -right;                             break;
				case 1: right = glm::vec3(-right.y,  right.x, 0.f); break;
				case 2: right = glm::vec3( right.y, -right.y, 0.f); break;
				default:                                            break;
			}

			right *= 25.f;

			config.SpawnLocationX = transform.Position.x + right.x;
			config.SpawnLocationY = transform.Position.y + right.y;
			config.EnemyType = iw::randf() > .4 ? BOMB : FIGHTER;
			config.TargetEntity = ship.Target;

			Bus->push<SpawnEnemy_Event>(config);
		}
	}

	for (auto [entity, transform, ship, base] : entities().query<iw::Transform, Enemy, Base_Enemy>().with_entity())
	{
		if (!ship.Target.is_alive())
		{
			return;
		}

		Throwable* closestThrowable = nullptr;
		float closestDist = FLT_MAX;

		for (auto [entity, throwTransform, throwable] : entities().query<iw::Transform, Throwable>().with_entity())
		{
			float dist = glm::distance(transform.Position, throwTransform.Position);

			if (dist < 150
				&& dist < closestDist)
			{
				closestDist = dist;
				closestThrowable = &throwable;
			}
		}

		if (    closestThrowable 
			&& !closestThrowable->Held)
		{
			closestThrowable->Held = true;
			closestThrowable->Time  = 1.f;
			closestThrowable->Timer = 0.f;
			closestThrowable->ThrowRequestor = entity;
			closestThrowable->ThrowTarget = ship.Target;
		}
	}

	for (auto [entity, transform, ship, boss] : entities().query<iw::Transform, Enemy, Boss_1_Enemy>().with_entity())
	{
		// need to design how this boss will fight you...
		
		if (boss.EnergyBall->CanFire())
		{
			// should materialize the energy ball infront of it

			glm::vec3 pos = ship.Target.get<iw::Transform>().Position;

			LightningConfig lighting;
			lighting.A = entity;
			lighting.TargetX = pos.x;
			lighting.TargetY = pos.y;
			lighting.ArcSize = 10;
			lighting.Space = Space;
			lighting.Task = Task;
			lighting.DelayPerCell = 0.005f;

			DrawLightning(sand, lighting);

			ShotInfo shot = boss.EnergyBall->GetShot(entity, pos.x, pos.y);

			Bus->push<SpawnProjectile_Event>(shot);
			Bus->push<PlaySound_Event>(boss.EnergyBall->Audio);
		}
	}
}

bool EnemySystem::On(iw::ActionEvent& e)
{
	switch (e.Action)
	{
		case SPAWN_ENEMY:
		{
			SpawnEnemy(e.as<SpawnEnemy_Event>().Config);
			break;
		}
	}

	return false;
}

void EnemySystem::SpawnEnemy(SpawnEnemy_Config& config)
{
	archetype archetype = make_archetype<Flocker, CorePixels, Enemy>();
	entity entity;

	component_list components = {
		make_component<Flocker>(),
		make_component<CorePixels>(),
		make_component<Enemy>()
	};

	iw::ref<iw::Texture> tex;

	switch (config.EnemyType)
	{
		case FIGHTER: 
		{
			components.add<Fighter_Enemy>();
			components.add<iw::Circle>();
			tex = A_texture_enemy_fighter;
			break;
		}
		case BOMB: 
		{
			components.add<Bomb_Enemy>();
			components.add<Throwable>();
			components.add<iw::Circle>();
			tex = A_texture_enemy_bomb;
			break;
		}
		case STATION:
		{
			components.add<Station_Enemy>();
			components.add<iw::MeshCollider2>();
			tex = A_texture_enemy_station;
			break;
		}
		case BASE:
		{
			components.add<Base_Enemy>();
			components.add<iw::MeshCollider2>();
			tex = A_texture_enemy_base;
			break;
		}
		case BOSS_1:
		{
			components.add<Boss_1_Enemy>();
			components.add<iw::MeshCollider2>();
			components.remove<Flocker>();
			tex = A_texture_enemy_boss_1;
		}
	}

	entity = MakeTile(sand->m_sandLayerIndex, *tex, components);
	AddEntityToPhysics(entity, Physics);

	iw::Rigidbody& rigidbody = entity.get<iw::Rigidbody>();
	iw::Transform& transform = entity.get<iw::Transform>();
	Enemy&         enemy     = entity.get<Enemy>();
	CorePixels&    core      = entity.get<CorePixels>();

	core.TimeWithoutCore = 0.f;

	transform.Position.x = config.SpawnLocationX;
	transform.Position.y = config.SpawnLocationY;

	rigidbody.SetTransform(&transform);
	rigidbody.SetMass(10);
	rigidbody.IsTrigger = true;

	enemy.Target = config.TargetEntity;

	switch (config.EnemyType)
	{
		case FIGHTER: 
		{
			Fighter_Enemy& fighter = entity.get<Fighter_Enemy>();
			fighter.Weapon = MakeLaser_Cannon_Enemy();

			enemy.ExplosionPower = 200;

			break;
		}
		case BOMB: 
		{
			Bomb_Enemy& bomb = entity.get<Bomb_Enemy>();
			bomb.TimeToExplode = .5f;
			bomb.RadiusToExplode = 30;

			rigidbody.AngularVelocity.z = .9f;

			enemy.ExplosionPower = 200;

			break;
		}
		case STATION:
		{
			Station_Enemy& station = entity.get<Station_Enemy>();
			station.Timer.SetTime("spawn", 2, .5);

			rigidbody.SetMass(100);
			//rigidbody->AngularVelocity.z = .1f;
			//rigidbody->Transform.Rotation.z = .4;

			enemy.ExplosionPower = 300;

			break;
		}
		case BASE:
		{
			Base_Enemy& station = entity.get<Base_Enemy>();

			rigidbody.SetMass(1000);
			rigidbody.AngularVelocity.z = .1f;

			enemy.ExplosionPower = 1200;

			break;
		}
		case BOSS_1:
		{
			Boss_1_Enemy& boss = entity.get<Boss_1_Enemy>();
			boss.EnergyBall = MakeWattz_Cannon();
			boss.Timer.SetTime("fire_ball", 1.f);

			rigidbody.IsSimulated = false;
			entity.get<iw::Tile>().m_dontRemoveCells = true;
		}
	}

	if (entity.has<Flocker>())
	{
		Flocker& flocker = entity.get<Flocker>();
		flocker.Target.x = config.TargetLocationX;
		flocker.Target.y = config.TargetLocationY;

		switch (config.EnemyType)
		{
			case BOMB: 
			{
				flocker.SpeedNearTarget = 1;
				break;
			}
			case STATION:
			{
				flocker.Speed = 25;
				break;
			}
			case BASE:
			{
				flocker.Speed = 25;
				break;
			}
		}
	}

	entity.on_destroy(
		[=](::entity entity)
		{
			std::vector<std::pair<ItemType, float>> item_weights {
				{ ItemType::HEALTH,         50 },
				{ ItemType::LASER_CHARGE,   50 },
				{ ItemType::WEAPON_MINIGUN, 3 },
				{ ItemType::WEAPON_BOLTZ,   2 },
				{ ItemType::WEAPON_WATTZ,   2 },
			};

			iw::Transform& transform = entity.get<iw::Transform>();

			SpawnItem_Config config;
			config.Item = iw::choose(item_weights);
			config.X = transform.Position.x;
			config.Y = transform.Position.y;
			config.AngularSpeed = iw::randf() * 25 + 10;

			switch (config.Item)
			{
				case ItemType::HEALTH: 
				case ItemType::LASER_CHARGE:   config.Amount = iw::randi(5) + 1; break;
		
				case ItemType::WEAPON_WATTZ:
				case ItemType::WEAPON_BOLTZ:
				case ItemType::WEAPON_MINIGUN: config.Amount = 1;
			}

			Bus->push<SpawnItem_Event>(config);

			// spawn copy of just display
			::entity tileCopy = MakeTile(sand->m_sandLayerIndex, entity.get<iw::Tile>().m_sprite, { make_component<iw::MeshCollider2>() });
			AddEntityToPhysics(tileCopy, Physics);

			tileCopy.set<iw::Transform>(transform);
			tileCopy.get<iw::Rigidbody>().SetTransform(&transform);
		}
	);

	Bus->push<CreatedCoreTile_Event>(entity);
}
