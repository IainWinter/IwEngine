#include "Systems/Enemy_System.h"

void EnemySystem::FixedUpdate()
{
	Space->Query<iw::Transform, EnemyShip>().Each(
		[&](
			iw::EntityHandle handle,
			iw::Transform* transform,
			EnemyShip* enemy)
		{
			if (   enemy->Weapon->CanFire()
				&& enemy->ShootAt.Alive())
			{
				glm::vec3 pos = enemy->ShootAt.Find<iw::Transform>()->Position;
				ShotInfo shot = enemy->Weapon->GetShot(Space->GetEntity(handle), pos.x, pos.y);
				Bus->send<SpawnProjectile_Event>(shot);
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
			SpawnEnemy(e.as<SpawnEnemy_Event>());
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
					Space->Query<EnemyShip>().Each([&](iw::EntityHandle handle, EnemyShip*) {
						Space->QueueEntity(handle, iw::func_Destroy);
					});
					break;
			}

			break;
		}
	}

	return false;
}

void EnemySystem::SpawnEnemy(SpawnEnemy_Event& config)
{
	iw::Entity e = sand->MakeTile<iw::Circle, EnemyShip, Flocker, CorePixels>("textures/SpaceGame/enemy.png", true);

	CorePixels*    core      = e.Set<CorePixels>();
	iw::Transform* transform = e.Find<iw::Transform>();
	iw::Rigidbody* rigidbody = e.Find<iw::Rigidbody>();
	iw::Circle*    collider  = e.Find<iw::Circle>();

	core->TimeWithoutCore = 0.f;

	transform->Position.x = config.SpawnLocationX;
	transform->Position.y = config.SpawnLocationY;

	rigidbody->SetTransform(transform);
	rigidbody->SetMass(10);

	collider->Radius = 6;

	EnemyShip* ship    = e.Set<EnemyShip>();
	Flocker*   flocker = e.Set<Flocker>();

	ship->Weapon = MakeLaser_Cannon_Enemy();
	ship->ShootAt = config.ShootAt;

	flocker->Target.x = config.TargetLocationX;
	flocker->Target.y = config.TargetLocationY;
}

void EnemySystem::DestroyEnemy(iw::Entity entity)
{
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
				
	iw::Cell smoke = iw::Cell::GetDefault(iw::CellType::SMOKE);

	for (int y = -5; y < 5; y++)
	for (int x = -5; x < 5; x++)
	{
		int px = x + transform->Position.x;
		int py = y + transform->Position.y;
		smoke.life = iw::randf() * 5;

		sand->m_world->SetCell(px, py, smoke);
	}

	Space->QueueEntity(entity.Handle, iw::func_Destroy);
}
