#include "Systems/Enemy_System.h"

void EnemySystem::FixedUpdate()
{
	Space->Query<iw::Transform, EnemyShip>().Each(
		[&](
			iw::EntityHandle handle,
			iw::Transform* transform,
			EnemyShip* enemy)
		{
			if (enemy->Weapon->CanFire())
			{
				iw::Entity ent = iw::Entity(handle, Space.get());
				glm::vec3 pos = enemy->ShootAt.Find<iw::Transform>()->Position;

				ShotInfo shot = enemy->Weapon->GetShot(ent, pos.x, pos.y);
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
	}

	return false;
}

void EnemySystem::SpawnEnemy(SpawnEnemy_Event& config)
{
	iw::Entity e = sand->MakeTile<iw::Circle, EnemyShip, Flocker>("textures/SpaceGame/enemy.png", true);

	iw::Transform* t = e.Set<iw::Transform>();
	iw::Rigidbody* r = e.Set<iw::Rigidbody>();
	iw::Circle*    c = e.Set<iw::Circle>();

	t->Position.x = config.SpawnLocationX;
	t->Position.y = config.SpawnLocationY;

	r->SetTransform(t);

	c->Radius = 6;

	EnemyShip* s = e.Set<EnemyShip>();
	Flocker*   f = e.Set<Flocker>();

	s->Weapon = MakeLaser_Cannon_Enemy();
	s->ShootAt = config.ShootAt;

	f->Target.x = config.TargetLocationX;
	f->Target.y = config.TargetLocationY;
}
