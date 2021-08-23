#include "Systems/Enemy_System.h"

void EnemySystem::FixedUpdate()
{
	Space->Query<iw::Transform, EnemyShip>().Each(
		[&](
			iw::EntityHandle entity,
			iw::Transform* transform,
			EnemyShip* enemy)
		{
			enemy->Timer.TickFixed();

			//if (enemy->Timer.Can("shoot"))
			//{
			//	iw::Transform* target = enemy->ShootAt.Find<iw::Transform>();

			//	auto [x, y, dx, dy] = GetShot(
			//		transform->Position.x, transform->Position.y, 
			//		target->   Position.x, target->   Position.y, 444, 10);

			//	float speed = sqrt(dx*dx + dy*dy);
			//	dx += iw::randfs() * speed * .05f;
			//	dy += iw::randfs() * speed * .05f;

			//	Bus->send<SpawnProjectile_Event>(x, y, dx, dy, ProjectileType::LASER);
			//}
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
	}

	return false;
}

void EnemySystem::SpawnEnemy(SpawnEnemy_Event& config)
{
	iw::Entity e = sand->MakeTile<iw::Circle, EnemyShip, Flocker>("textures/SpaceGame/enemy.png", true);

	iw::Transform* t = e.Set<iw::Transform>();
	iw::Rigidbody* r = e.Set<iw::Rigidbody>();

	t->Position.x = config.SpawnLocationX;
	t->Position.y = config.SpawnLocationY;

	r->SetTransform(t);

	EnemyShip* s = e.Set<EnemyShip>();
	Flocker*   f = e.Set<Flocker>();

	s->Timer.SetTime("shoot", 1, 1);
	s->ShootAt = config.ShootAt;

	f->Target.x = config.TargetLocationX;
	f->Target.y = config.TargetLocationY;
}
