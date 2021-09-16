#include "Systems/Enemy_System.h"

void EnemySystem::FixedUpdate()
{
	Space->Query<iw::Transform, Enemy, Fighter_Enemy>().Each(
		[&](
			iw::EntityHandle handle,
			iw::Transform* transform,
			Enemy*         ship,
			Fighter_Enemy* fighter)
		{
			if (   fighter->Weapon->CanFire()
				&& ship->Target.Alive())
			{
				glm::vec3 pos = ship->Target.Find<iw::Transform>()->Position;
				ShotInfo shot = fighter->Weapon->GetShot(Space->GetEntity(handle), pos.x, pos.y);
				Bus->push<SpawnProjectile_Event>(shot);
			}
		}
	);

	Space->Query<iw::Transform, Flocker, Enemy, Bomb_Enemy>().Each(
		[&](
			iw::EntityHandle handle,
			iw::Transform* transform,
			Flocker* flocker,
			Enemy*      ship,
			Bomb_Enemy* bomb)
		{
			// maybe put some timer or something

			glm::vec3 pos = ship->Target.Find<iw::Transform>()->Position;
			
			// set flocking target to target
			flocker->Target = glm::vec2(pos.x, pos.y);

			// start explosion if close to target
			if (   !bomb->Explode 
				&& glm::distance(pos, transform->Position) < bomb->RadiusToExplode)
			{
				bomb->Explode = true;
			}

			if (bomb->Explode)
			{
				bomb->TimeToExplode -= iw::FixedTime();

				if (bomb->TimeToExplode <= 0.f)
				{
					SpawnExplosion_Config config;
					config.SpawnLocationX = transform->Position.x;
					config.SpawnLocationY = transform->Position.y;
					config.ExplosionPower = 30;
					config.ExplosionRadius = 30;

					Bus->push<SpawnExplosion_Event>(config);
					Space->QueueEntity(handle, iw::func_Destroy);
				}
			}
		}
	);

	Space->Query<iw::Transform, Enemy, Station_Enemy>().Each(
		[&](
			iw::EntityHandle handle,
			iw::Transform* transform,
			Enemy*         ship,
			Station_Enemy* station)
		{
			station->timer.TickFixed();
			if (station->timer.Can("spawn"))
			{
				SpawnEnemy_Config config;

				config.SpawnLocationX = transform->Position.x;
				config.SpawnLocationX = transform->Position.y;
				config.EnemyType = BOMB;
				config.TargetEntity = ship->Target;

				Bus->push<SpawnEnemy_Event>(config);
			}
		}
	);
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
		case CORE_EXPLODED: {
			DestroyEnemy(e.as<CoreExploded_Event>().Entity);
			break;
		}
		case STATE_CHANGE:
		{
			StateChange_Event& event = e.as<StateChange_Event>();

			switch (event.State)
			{
				case RUN_STATE:
					Space->Query<Enemy>().Each([&](iw::EntityHandle handle, Enemy*) {
						Space->QueueEntity(handle, iw::func_Destroy);
					});
					break;
			}

			break;
		}
	}

	return false;
}

void EnemySystem::SpawnEnemy(SpawnEnemy_Config& config)
{
	iw::Archetype archetype = Space->CreateArchetype<Flocker, CorePixels, Enemy>();
	iw::ref<iw::Texture> texture;

	switch (config.EnemyType)
	{
		case FIGHTER: 
		{
			Space->AddComponent<Fighter_Enemy>(archetype);
			texture = A_texture_enemy_fighter;
			break;
		}
		case BOMB: 
		{
			Space->AddComponent<Bomb_Enemy>(archetype);
			texture = A_texture_enemy_bomb;
			break;
		}
		case STATION:
		{
			Space->AddComponent<Station_Enemy>(archetype);
			texture = A_texture_enemy_station;
			break;
		}
	}

	iw::Entity entity = sand->MakeTile<iw::Circle>(texture, true, &archetype);

	iw::Rigidbody* rigidbody = entity.Find<iw::Rigidbody>();
	iw::Transform* transform = entity.Find<iw::Transform>();
	Flocker*       flocker   = entity.Set<Flocker>();
	Enemy*         enemy     = entity.Set<Enemy>();
	CorePixels*    core      = entity.Set<CorePixels>();

	core->TimeWithoutCore = 0.f;
	
	flocker->Target.x = config.TargetLocationX;
	flocker->Target.y = config.TargetLocationY;

	transform->Position.x = config.SpawnLocationX;
	transform->Position.y = config.SpawnLocationY;

	rigidbody->SetTransform(transform);
	rigidbody->SetMass(10);

	enemy->Target = config.TargetEntity;
	
	switch (config.EnemyType)
	{
		case FIGHTER: 
		{
			Fighter_Enemy* fighter = entity.Set<Fighter_Enemy>();
			fighter->Weapon = MakeLaser_Cannon_Enemy();

			break;
		}
		case BOMB: 
		{
			Bomb_Enemy* bomb = entity.Set<Bomb_Enemy>();
			bomb->TimeToExplode = .5f;
			bomb->RadiusToExplode = 30;

			flocker->SpeedNearTarget = 1;
			rigidbody->AngularVelocity.z = .9f;

			break;
		}
		case STATION:
		{
			Station_Enemy* station = entity.Set<Station_Enemy>();
			station->timer.SetTime("spawn", 2, .5);

			rigidbody->AngularVelocity.z = .1f;
			rigidbody->SetMass(100);
			flocker->Speed = 4;

			break;
		}
	}

	Bus->push<CreatedCoreTile_Event>(entity);
}

void EnemySystem::DestroyEnemy(iw::Entity entity)
{
	if (!entity.Has<Enemy>()) return;

	std::vector<std::pair<ItemType, float>> item_weights {
		{ HEALTH,         50 },
		{ LASER_CHARGE,   50 },
		{ WEAPON_MINIGUN, 10 }
	};

	iw::Transform* transform = entity.Find<iw::Transform>();

	SpawnItem_Config config;
	config.Item = iw::choose(item_weights);
	config.X = transform->Position.x;
	config.Y = transform->Position.y;

	switch (config.Item)
	{
		case ItemType::HEALTH: 
		case ItemType::LASER_CHARGE:   config.Amount = iw::randi(5) + 1; break;
		case ItemType::WEAPON_MINIGUN: config.Amount = 1;
	}

	Bus->push<SpawnItem_Event>(config);
	Space->QueueEntity(entity.Handle, iw::func_Destroy);
}
