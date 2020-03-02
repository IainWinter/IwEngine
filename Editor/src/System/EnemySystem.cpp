#include "Systems/EnemySystem.h"
#include "Components/Bullet.h"
#include "iw/engine/Time.h"
#include "iw/graphics/Model.h"
#include "iw/input/Devices/Keyboard.h"
#include "iw/physics/AABB.h"

#include "IW/physics/Collision/SphereCollider.h";
#include "IW/physics/Dynamics/Rigidbody.h";
#include <Components\Player.h>

#include "Events/ActionEvents.h"
#include "iw/audio/AudioSpaceStudio.h"

EnemySystem::EnemySystem()
	: iw::System<iw::Transform, Enemy>("Enemy")
	, m_bulletModel(nullptr)
	, m_enemyCount(0)
	, m_levelResetTimer(0)
{}

int EnemySystem::Initialize() {
	m_bulletModel = Asset->Load<iw::Model>("Sphere");
	
	return 0;
}

void EnemySystem::Update(
	iw::EntityComponentArray& view)
{
	m_enemyCount = 0;

	for (auto entity : view) {
		auto [transform, enemy] = entity.Components.Tie<Components>();

		m_enemyCount++;

		transform->Rotation *= iw::quaternion::from_euler_angles(0, -iw::Time::DeltaTime(), 0);

		if (  !enemy->HasShot
			&& enemy->Timer <= 0)
		{
			enemy->HasShot = true;

			switch (enemy->Type) {
				case EnemyType::SPIN: {
					enemy->Rotation = fmod(enemy->Rotation + enemy->Speed, iw::Pi2);

					SpawnBullet(
						enemy->Bullet,
						transform->Position,
						iw::quaternion::from_euler_angles(0, enemy->Rotation, 0)
					);

					break;
				}
				case EnemyType::CIRCLE: {
					for (float i = 1; i < iw::Pi2 / enemy->Speed; i++) {
						SpawnBullet(
							enemy->Bullet,
							transform->Position,
							iw::quaternion::from_euler_angles(0, enemy->Rotation + enemy->Speed * i, 0)
						);
					}

					break;
				}
			}
		}

		if (enemy->Timer <= -enemy->CooldownTime) {
			enemy->HasShot = false;
			enemy->Timer = enemy->FireTime;
		}

		if (enemy->Timer >= -enemy->CooldownTime) {
			enemy->Timer -= iw::Time::DeltaTime();
		}
	}

	if (m_enemyCount == 0) {
		m_levelResetTimer += iw::Time::DeltaTime();
		if (m_levelResetTimer > 1.0f) {
			m_levelResetTimer = 0;
			Bus->push<iw::NextLevelEvent>();
		}
	}
}

bool EnemySystem::On(
	iw::CollisionEvent& event)
{
	iw::Entity a = Space->FindEntity(event.ObjA);
	if (a == iw::EntityHandle::Empty) {
		a = Space->FindEntity<iw::Rigidbody>(event.ObjA);
	}

	iw::Entity b = Space->FindEntity(event.ObjB);
	if (b == iw::EntityHandle::Empty) {
		b = Space->FindEntity<iw::Rigidbody>(event.ObjB);
	}

	if (   a == iw::EntityHandle::Empty
		|| b == iw::EntityHandle::Empty)
	{
		return false;
	}

	iw::Entity player;
	iw::Entity enemy;
	if (   a.HasComponent<Player>()
		&& b.HasComponent<Enemy>())
	{
		player = a;
		enemy  = b;
	}

	else if (b.HasComponent<Player>()
		&&   a.HasComponent<Enemy>())
	{
		player = b;
		enemy  = a;
	}

	if (   player != iw::EntityHandle::Empty
		&& enemy  != iw::EntityHandle::Empty)
	{
		Player* playerComponent = player.FindComponent<Player>();
		if (playerComponent->Timer > 0) {
			QueueDestroyEntity(enemy.Index());
			Audio->AsStudio()->CreateInstance("enemyDeath");
		}

		else {
			playerComponent->Health -= 1;
		}
	}

	return false;
}

void EnemySystem::SpawnBullet(
	Bullet prefab,
	iw::vector3 position,
	iw::quaternion rot)
{
	iw::Entity bullet = Space->CreateEntity<iw::Transform, iw::Model, iw::SphereCollider, iw::Rigidbody, Bullet>();
	
	                        bullet.SetComponent<iw::Model>         (*m_bulletModel);
	Bullet*             b = bullet.SetComponent<Bullet>            (prefab);
	iw::Transform*      t = bullet.SetComponent<iw::Transform>     (position + iw::vector3(sqrt(2), 0, 0) * rot, iw::vector3(.25f));
	iw::SphereCollider* s = bullet.SetComponent<iw::SphereCollider>(iw::vector3::zero, .25f);
	iw::Rigidbody*      r = bullet.SetComponent<iw::Rigidbody>     ();

	b->initialVelocity = iw::vector3::unit_x * rot * prefab.Speed;

	r->SetMass(1);
	r->SetCol(s);
	r->SetTrans(t);
	r->SetVelocity(b->initialVelocity);
	r->SetSimGravity(false);
	r->SetRestitution(1);
	r->SetIsTrigger(true);
	r->SetIsLocked(iw::vector3(0, 1, 0));
	r->SetLock(iw::vector3(0, 1, 0));

	Physics->AddRigidbody(r);
}
