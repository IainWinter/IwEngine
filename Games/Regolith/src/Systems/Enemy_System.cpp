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
		case RUN_GAME: {
			Space->Query<EnemyShip>().Each([&](iw::EntityHandle handle, EnemyShip*) {
				Space->QueueEntity(handle, iw::func_Destroy);
			});
			break;
		}
	}

	return false;
}

void EnemySystem::SpawnEnemy(SpawnEnemy_Event& config)
{
	iw::Entity e = sand->MakeTile<iw::Circle, EnemyShip, Flocker, CorePixels>("textures/SpaceGame/enemy.png", true);

	CorePixels*    core      = e.Set<CorePixels>();
	iw::Transform* transform = e.Set<iw::Transform>();
	iw::Rigidbody* rigidbody = e.Set<iw::Rigidbody>();
	iw::Circle*    collider  = e.Set<iw::Circle>();

	core->TimeWithoutCore = 0.f;

	transform->Position.x = config.SpawnLocationX;
	transform->Position.y = config.SpawnLocationY;

	rigidbody->SetTransform(transform);

	collider->Radius = 6;

	EnemyShip* s = e.Set<EnemyShip>();
	Flocker*   f = e.Set<Flocker>();

	s->Weapon = MakeLaser_Cannon_Enemy();
	s->ShootAt = config.ShootAt;

	f->Target.x = config.TargetLocationX;
	f->Target.y = config.TargetLocationY;
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
