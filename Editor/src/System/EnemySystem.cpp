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

EnemySystem::EnemySystem(
	iw::Entity& player)
	: iw::System<iw::Transform, Enemy>("Enemy")
	, m_enemyCount(0)
	, m_levelResetTimer(0)
	, player(player)
{}

int EnemySystem::Initialize() {
	iw::Mesh sphere = Asset->Load<iw::Model>("Sphere")->GetMesh(0).MakeInstance();

	sphere.Material()->Set("baseColor", iw::Color::From255(0, 213, 255, 191));

	m_bulletModel.AddMesh(sphere);

	return 0;
}

void EnemySystem::Update(
	iw::EntityComponentArray& view)
{
	m_enemyCount = 0;

	for (auto entity : view) {
		auto [transform, enemy] = entity.Components.Tie<Components>();

		m_enemyCount++;

		switch (enemy->Type) {
			// spin to win ez
			case EnemyType::SPIN: {
				transform->Rotation *= iw::quaternion::from_euler_angles(0, -iw::Time::DeltaTimeScaled(), 0);
				break;
			}
			// charge over time and then shoot a circle
			case EnemyType::CIRCLE: {
				if (enemy->Timer < enemy->ChargeTime) {
					float speed = 1.0f / (enemy->ChargeTime - enemy->Timer);

					if (speed > 30.0f) {
						speed = 30.0f;
					}

					enemy->RotSpeed += 5 * speed * iw::Time::DeltaTimeScaled();
				}

				else {
					enemy->RotSpeed *= 1 - 20 * iw::Time::DeltaTimeScaled();
				}

				transform->Rotation *= iw::quaternion::from_euler_angles(0, enemy->RotSpeed * iw::Time::DeltaTimeScaled(), 0);
				break;
			}
			// lmao this one breaks the story
			// 
			case EnemyType::SEEK: {
				iw::vector3 target = player.Find<iw::Transform>()->Position;
				float       dir    = atan2(target.z - transform->Position.z, target.x - transform->Position.x);

				transform->Rotation = iw::quaternion::from_euler_angles(0, dir, 0);
				
				// need to slerp

				break;
			}
			// this one secretly breaks the story xd
			// spins to player quickly and does 1 / 2 firing moves
			case EnemyType::MINI_BOSS_BOX_SPIN: {
				iw::vector3 target = player.Find<iw::Transform>()->Position;
				float       dir    = atan2(target.z - transform->Position.z, target.x - transform->Position.x);

				transform->Rotation = iw::quaternion::from_euler_angles(0, dir, 0);

				break;
			}
		}

		if (  !enemy->HasShot
			&& enemy->Timer > enemy->ChargeTime)
		{
			enemy->HasShot = true;

			switch (enemy->Type) {
				case EnemyType::SPIN: {
					iw::Transform* bullet = SpawnBullet(
						enemy->Bullet,
						transform->Position,
						iw::quaternion::from_euler_angles(0, enemy->Rotation, 0),
						entity.Index
					);

					transform->Parent()->AddChild(bullet);

					break;
				}
				case EnemyType::CIRCLE: {
					int count = roundf(iw::Pi2 / enemy->Speed);
					for (float i = 0; i < count; i++) {
						iw::Transform* bullet = SpawnBullet(
							enemy->Bullet,
							transform->Position,
							iw::quaternion::from_euler_angles(0, enemy->Rotation + enemy->Speed * i, 0),
							entity.Index
						);

						transform->Parent()->AddChild(bullet);
					}

					break;
				}
				case EnemyType::SEEK: {
					iw::Transform* bullet = SpawnBullet(
						enemy->Bullet,
						transform->Position,
						transform->Rotation.inverted(),
						entity.Index
					);

					transform->Parent()->AddChild(bullet);

					break;
				}
				case EnemyType::MINI_BOSS_BOX_SPIN: {
					float time = enemy->FireTime - enemy->ChargeTime;

					if      (enemy->Timer >= enemy->ChargeTime + time / 2.0f)
					{
						if (enemy->Timer2 == 0.0f) {
							float rot = iw::hPi + iw::Pi / 8 * sin(iw::Time::TotalTime() * 5);

							for (int i = 0; i <= enemy->Speed; i++) {
								iw::quaternion offset = iw::quaternion::from_euler_angles(0, -rot + iw::Pi / enemy->Speed * i, 0);

								iw::Transform* bullet = SpawnBullet(
									enemy->Bullet,
									transform->Position,
									transform->Rotation.inverted() * offset,
									entity.Index
								);
							}
						}

						enemy->Timer2 += iw::Time::DeltaTimeScaled();

						if (enemy->Timer2 > 0.15f) {
							enemy->Timer2 = 0.0f;
						}

						if (enemy->Timer <= enemy->ChargeTime + time / 4 * 3) {
							enemy->HasShot = false;
						}
					}

					else if (enemy->Timer >= enemy->ChargeTime + time / 4)
					{
						LOG_DEBUG << enemy->Timer;
					}

					else
					{
						//if (iw::randf() >= 0.5f) {
							enemy->Timer = enemy->ChargeTime + time / 2;
						//}

						//else {
						//	enemy->Timer = enemy->ChargeTime + time / 4;
						//}

						enemy->HasShot = false;
					}
				}
			}

			enemy->Rotation = fmod(enemy->Rotation + enemy->Speed, iw::Pi2);
		}

		else if (enemy->Timer >= enemy->FireTime) {
			enemy->HasShot = false;
			enemy->Timer   = 0;
			enemy->Timer2  = 0;
		}

		enemy->Timer  += iw::Time::DeltaTimeScaled();

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
	if (   a.Has<Player>()
		&& b.Has<Enemy>())
	{
		player = a;
		enemy  = b;
	}

	else if (b.Has<Player>()
		&&   a.Has<Enemy>())
	{
		player = b;
		enemy  = a;
	}

	if (   player != iw::EntityHandle::Empty
		&& enemy  != iw::EntityHandle::Empty)
	{
		Player* playerComponent = player.Find<Player>();
		if (playerComponent->Timer > 0) {
			Audio->AsStudio()->CreateInstance("enemyDeath");

			iw::Transform* transform = enemy.Find<iw::Transform>();

			Bus->push<SpawnEnemyDeath>(enemy.Find<iw::Transform>()->Position, transform->Parent());

			transform->SetParent(nullptr); // got moved to enemtdeathcirclesystem but should be in seperate system that cleans up destroied entities (remove from physics / transform tree)
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
	iw::quaternion rot,
	int index)
{
	iw::Entity bullet = Space->CreateEntity<iw::Transform, iw::Model, iw::SphereCollider, iw::Rigidbody, Bullet>();
	
	                        bullet.Set<iw::Model>         (m_bulletModel);
	Bullet*             b = bullet.Set<Bullet>            (prefab);
	iw::Transform*      t = bullet.Set<iw::Transform>     (position + iw::vector3(sqrt(2), 0, 0) * rot, iw::vector3(.25f));
	iw::SphereCollider* s = bullet.Set<iw::SphereCollider>(iw::vector3::zero, 0.5f);
	iw::Rigidbody*      r = bullet.Set<iw::Rigidbody>     ();

	b->initialVelocity = iw::vector3::unit_x * rot * prefab.Speed;

	b->enemyIndex = index;

	r->SetMass(1);
	r->SetCol(s);
	r->SetTrans(t);
	r->SetVelocity(b->initialVelocity);
	r->SetSimGravity(false);
	r->SetRestitution(0.1f);
	r->SetIsTrigger(true);
	r->SetIsLocked(iw::vector3(0, 1, 0));
	r->SetLock(iw::vector3(0, 1, 0));

	Physics->AddRigidbody(r);

	return t;
}
