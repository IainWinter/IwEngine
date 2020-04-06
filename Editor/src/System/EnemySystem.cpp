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
	, m_bulletModel(nullptr)
	, m_enemyCount(0)
	, m_levelResetTimer(0)
	, player(player)
{}

int EnemySystem::Initialize() {
	iw::Mesh*    mesh = Asset->Load<iw::Model>("Sphere")->Meshes[0].Instance();
	iw::Material mat  = mesh->Material->Instance();

	iw::Model bullet = { mesh, 1 };

	m_bulletModel = Asset->Give<iw::Model>("Bullet", &bullet);

	mat.Set("albedo", iw::Color::From255(0, 213, 255, 191));
	//mat.SetTransparency(iw::Transparency::ADD);
	//mat.SetCastShadows(false);

	m_bulletModel->Meshes[0].Material = REF<iw::Material>(mat);

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
			case EnemyType::SPIN: {
				transform->Rotation *= iw::quaternion::from_euler_angles(0, -iw::Time::DeltaTime(), 0);
				break;
			}
			case EnemyType::CIRCLE: {
				if (enemy->Timer < enemy->ChargeTime) {
					float speed = 1.0f / (enemy->ChargeTime - enemy->Timer);

					if (speed > 30.0f) {
						speed = 30.0f;
					}

					enemy->RotSpeed += 5 * speed * iw::Time::DeltaTime();
				}

				else {
					enemy->RotSpeed *= 1 - 20 * iw::Time::DeltaTime();
				}

				transform->Rotation *= iw::quaternion::from_euler_angles(0, enemy->RotSpeed * iw::Time::DeltaTime(), 0);
				break;
			}
			case EnemyType::SEEK: {
				iw::vector3 target = player.FindComponent<iw::Transform>()->Position;
				float       dir    = atan2(target.z - transform->Position.z, target.x - transform->Position.x) /*+ iw::Pi*/;

				transform->Rotation = //iw::lerp(transform->Rotation,
					  iw::quaternion::from_euler_angles(0, dir, 0) 
					//* iw::quaternion::from_euler_angles(0, iw::Pi, 0), iw::Time::DeltaTime()
				/*).normalized()*/;
				
				// need to slerp

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
			}

			enemy->Rotation = fmod(enemy->Rotation + enemy->Speed, iw::Pi2);
		}

		else if (enemy->Timer >= enemy->FireTime) {
			enemy->HasShot = false;
			enemy->Timer   = 0;
		}

		else {
			enemy->Timer += iw::Time::DeltaTime();
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

			iw::Transform* transform = enemy.FindComponent<iw::Transform>();

			Bus->push<SpawnEnemyDeath>(enemy.FindComponent<iw::Transform>()->Position, transform->Parent());

			transform->SetParent(nullptr); // got moved to enemtdeathcirclesystem but should be in seperate system that cleans up destroied entities (remove from physics / transform tree)
			Space->DestroyEntity(enemy.Index());
		}
	}

	return false;
}

bool trigger = true;

bool EnemySystem::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::GOTO_NEXT_LEVEL): {
			if (e.as<GoToNextLevelEvent>().LevelName == "models/block/forest12.dae") {
				trigger = false;
			}

			else {
				trigger = true;
			}
		}
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
	
	                        bullet.SetComponent<iw::Model>         (*m_bulletModel);
	Bullet*             b = bullet.SetComponent<Bullet>            (prefab);
	iw::Transform*      t = bullet.SetComponent<iw::Transform>     (position + iw::vector3(sqrt(2), 0, 0) * rot, iw::vector3(.25f));
	iw::SphereCollider* s = bullet.SetComponent<iw::SphereCollider>(iw::vector3::zero, 1);
	iw::Rigidbody*      r = bullet.SetComponent<iw::Rigidbody>     ();

	b->initialVelocity = iw::vector3::unit_x * rot * prefab.Speed;

	b->enemyIndex = index;

	r->SetMass(1);
	r->SetCol(s);
	r->SetTrans(t);
	r->SetVelocity(b->initialVelocity);
	r->SetSimGravity(false);
	r->SetRestitution(0.1f);
	r->SetIsTrigger(trigger);
	r->SetIsLocked(iw::vector3(0, 1, 0));
	r->SetLock(iw::vector3(0, 1, 0));

	Physics->AddRigidbody(r);

	return t;
}
