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
	
	iw::ref<iw::Material> blue = REF<iw::Material>(m_bulletModel->Meshes[0].Material->Instance());

	blue->Set("albedo", iw::Color::From255(0, 213, 255, 128));

	m_bulletModel->Meshes[0].Material = blue;

	return 0;
}

void EnemySystem::Update(
	iw::EntityComponentArray& view)
{
	m_enemyCount = 0;

	for (auto entity : view) {
		auto [transform, enemy] = entity.Components.Tie<Components>();

		if (transform->Parent() == nullptr) {
			LOG_INFO << "herh";
		}

		m_enemyCount++;

		transform->Rotation *= iw::quaternion::from_euler_angles(0, -iw::Time::DeltaTime(), 0);

		if (  !enemy->HasShot
			&& enemy->Timer <= 0)
		{
			enemy->HasShot = true;

			switch (enemy->Type) {
				case EnemyType::SPIN: {
					iw::Transform* bullet = SpawnBullet(
						enemy->Bullet,
						transform->Position,
						iw::quaternion::from_euler_angles(0, enemy->Rotation, 0)
					);

					transform->Parent()->AddChild(bullet);

					break;
				}
				case EnemyType::CIRCLE: {
					for (float i = 1; i < iw::Pi2 / enemy->Speed; i++) {
						iw::Transform* bullet = SpawnBullet(
							enemy->Bullet,
							transform->Position,
							iw::quaternion::from_euler_angles(0, enemy->Rotation + enemy->Speed * i, 0)
						);

						transform->Parent()->AddChild(bullet);
					}

					break;
				}
			}

			enemy->Rotation = fmod(enemy->Rotation + enemy->Speed, iw::Pi2);
		}

		if (enemy->Timer <= -enemy->CooldownTime) {
			enemy->HasShot = false;
			enemy->Timer = enemy->FireTime;
		}

		if (enemy->Timer >= -enemy->CooldownTime) {
			enemy->Timer -= iw::Time::DeltaTime();
		}
	}

	if (   m_enemyCount == 0
		&& m_levelResetTimer < 0.2f)
	{
		m_levelResetTimer += iw::Time::DeltaTime();
		if (m_levelResetTimer > 0.2f) {
			Bus->push<UnlockLevelDoorEvent>();
		}
	}
}

bool EnemySystem::On(
	iw::CollisionEvent& e)
{
	iw::Entity a = Space->FindEntity(e.ObjA);
	if (a == iw::EntityHandle::Empty) {
		a = Space->FindEntity<iw::Rigidbody>(e.ObjA);
	}

	iw::Entity b = Space->FindEntity(e.ObjB);
	if (b == iw::EntityHandle::Empty) {
		b = Space->FindEntity<iw::Rigidbody>(e.ObjB);
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
			Audio->AsStudio()->CreateInstance("enemyDeath");

			enemy.FindComponent<iw::Transform>()->SetParent(nullptr);
			Space->DestroyEntity(enemy.Index());
		}
	}

	return false;
}

bool EnemySystem::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::START_LEVEL):
		case iw::val(Actions::RESET_LEVEL): {
			m_levelResetTimer = 0;
			break;
		}
	}

	return false;
}

iw::Transform* EnemySystem::SpawnBullet(
	Bullet prefab,
	iw::vector3 position,
	iw::quaternion rot)
{
	iw::Entity bullet = Space->CreateEntity<iw::Transform, iw::Model, iw::SphereCollider, iw::Rigidbody, Bullet>();
	
	                        bullet.SetComponent<iw::Model>         (*m_bulletModel);
	Bullet*             b = bullet.SetComponent<Bullet>            (prefab);
	iw::Transform*      t = bullet.SetComponent<iw::Transform>     (position + iw::vector3(sqrt(2), 0, 0) * rot, iw::vector3(.25f));
	iw::SphereCollider* s = bullet.SetComponent<iw::SphereCollider>(iw::vector3::zero, 1);
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

	return t;
}
