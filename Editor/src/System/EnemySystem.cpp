#include "Systems/EnemySystem.h"

#include "iw/engine/Time.h"
#include "iw/graphics/Model.h"
#include "iw/input/Devices/Keyboard.h"

#include "iw/physics/Collision/SphereCollider.h";
#include "iw/physics/Dynamics/Rigidbody.h";
#include "iw/physics/AABB.h"

#include "iw/audio/AudioSpaceStudio.h"

#include "Components/Bullet.h"
#include "Components/Player.h"
#include "Components/LevelDoor.h"
#include "Components/DontDeleteBullets.h"
#include "Components/EnemyDeathCircle.h"

#include "Events/ActionEvents.h"

EnemySystem::EnemySystem(
	iw::Entity& player,
	iw::Prefab& bullet)
	: iw::System<iw::Transform, Enemy>("Enemy")
	, m_enemyCount(0)
	, m_levelResetTimer(0)
	, player(player)
	, m_bullet(bullet)
{}

void EnemySystem::Update(
	iw::EntityComponentArray& view)
{
	unsigned enemyCount = 0;

	for (auto entity : view) {
		auto [transform, enemy] = entity.Components.Tie<Components>();

		iw::CollisionObject* obj = Space->FindComponent<iw::CollisionObject>(entity.Handle);
		if (!obj) {
			obj = Space->FindComponent<iw::Rigidbody>(entity.Handle);
		}

		enemyCount++;

		switch (enemy->Type) {
			// spin to win ez
			case EnemyType::SPIN: {
				obj->Trans().Rotation *= iw::quaternion::from_euler_angles(0, -iw::Time::DeltaTimeScaled(), 0);
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

				obj->Trans().Rotation *= iw::quaternion::from_euler_angles(0, enemy->RotSpeed * iw::Time::DeltaTimeScaled(), 0);

				break;
			}
			// lmao this one breaks the story
			// 
			case EnemyType::SEEK: {
				iw::vector3 target = player.Find<iw::Transform>()->WorldPosition();
				float       dir    = atan2(target.z - transform->Position.z, target.x - transform->Position.x);

				obj->Trans().Rotation = iw::quaternion::from_euler_angles(0, dir, 0);
				
				// need to slerp

				break;
			}
			// this one secretly breaks the story xd
			// spins to player quickly and does 1 / 2 firing moves
			case EnemyType::MINI_BOSS_FOREST:
			case EnemyType::MINI_BOSS_CANYON:
			case EnemyType::BOSS_FOREST: {
				// should rotate in same direction until at player
				if (enemy->Timer < enemy->ChargeTime) {
					iw::vector3 target = player.Find<iw::Transform>()->WorldPosition();
					float       dir    = atan2(target.z - transform->Position.z, target.x - transform->Position.x);

					obj->Trans().Rotation = iw::lerp(obj->Trans().Rotation, iw::quaternion::from_euler_angles(0, dir, 0), iw::Time::DeltaTime() * 4);
				}

				break;
			}
		}

		if (  !enemy->HasShot
			&& enemy->Timer > enemy->ChargeTime) // on ground lmao
		{
			enemy->HasShot = true;

			switch (enemy->Type) {
				case EnemyType::SPIN: {
					Bus->push<SpawnBulletEvent>(
						enemy->Bullet,
						transform->Position,
						iw::quaternion::from_euler_angles(0, enemy->Rotation, 0),
						transform->Parent()
					);

					break;
				}
				case EnemyType::CIRCLE: {
					int count = roundf(iw::Pi2 / enemy->Speed);
					for (float i = 0; i < count; i++) {
						Bus->push<SpawnBulletEvent>(
							enemy->Bullet,
							transform->Position,
							iw::quaternion::from_euler_angles(0, enemy->Rotation + enemy->Speed * i, 0),
							transform->Parent()
						);
					}

					break;
				}
				case EnemyType::SEEK: {
					Bus->push<SpawnBulletEvent>(
						enemy->Bullet,
						transform->Position,
						transform->Rotation.inverted(),
						transform->Parent()
					);

					break;
				}
			}

			enemy->Rotation = fmod(enemy->Rotation + enemy->Speed, iw::Pi2);
		}

		else if (enemy->Timer >= enemy->FireTime) {
			enemy->HasShot = false;
			enemy->Timer = 0;
			enemy->Timer2 = 0;
		}

		enemy->Timer += iw::Time::DeltaTimeScaled();
	}

	if (   enemyCount == 0
		&& m_levelResetTimer < 0.2f)
	{
		m_levelResetTimer += iw::Time::DeltaTime();
		if (m_levelResetTimer > 0.2f) {
			Bus->push<UnlockLevelDoorEvent>();
		}
	}

	m_enemyCount = enemyCount;
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
		case iw::val(Actions::SPAWN_ENEMY): {
			SpawnEnemyEvent& event = e.as<SpawnEnemyEvent>();

			iw::Transform* t = SpawnEnemy(event.Enemy, event.Position, event.Velocity);
			if (t) {
				event.Level->AddChild(t);
				event.SpawnedEnemy = Space->FindEntity(t);
			}

			break;
		}
	}

	return false;
}

iw::Transform* EnemySystem::SpawnEnemy(
	Enemy prefab,
	iw::vector3 position,
	iw::vector3 velocity)
{
	iw::Entity ent = Space->CreateEntity<iw::Transform, iw::Model, iw::SphereCollider, Enemy>();

	switch (prefab.Type) {
		case EnemyType::MINI_BOSS_FOREST: {
			EnemyBoss* boss = ent.Add<EnemyBoss>();
			boss->Actions.push_back({ 0, 2.5f, 0.02f });
			boss->Actions.push_back({ 1, 2.5f, 0.2f });

			break;
		}
		case EnemyType::MINI_BOSS_CANYON: {
			EnemyBoss* boss = ent.Add<EnemyBoss>();
			boss->Actions.push_back({ 4, 2.0f, 0.2f });
			boss->Actions.push_back({ 7, 1.5f, 0.0f, -1, 1 });

			break;
		}
		case EnemyType::BOSS_FOREST:
		{
			EnemyBoss* boss = ent.Add<EnemyBoss>();
			boss->Actions.push_back({ 2, 3.5f, 0.2f, 0 });
			boss->Actions.push_back({ 3, 2.4f, 0.4f });

			boss->FirstAction = 1;

			break;
		}
	}

	iw::CollisionObject* c;

	if (velocity.length_squared() == 0) {
		c = ent.Add<iw::CollisionObject>();
		Physics->AddCollisionObject(c);
	}

	else {
		iw::Rigidbody* r = ent.Add<iw::Rigidbody>();
		Physics->AddRigidbody(r);
		
		r->SetVelocity(velocity);
		r->SetIsStatic(false);
		r->SetSimGravity(true);

		r->SetStaticFriction(.5f);
		r->SetDynamicFriction(.5f);

		c = r;
	}

	Enemy*               e = ent.Set<Enemy>(prefab);
	iw::Transform*       t = ent.Set<iw::Transform>(position);
	iw::SphereCollider*  s = ent.Set<iw::SphereCollider>(iw::vector3::zero, 1.0f);
		
	switch (prefab.Type) {
		case EnemyType::MINI_BOSS_FOREST:
		case EnemyType::MINI_BOSS_CANYON:
		case EnemyType::BOSS_FOREST:
		{
			e->Health = 3;
			e->ScoreMultiple = 10;
			t->Scale = 0.75f;
			ent.Set<iw::Model>(*Asset->Load<iw::Model>("Box"));

			break;
		}
		default: {
			e->Health = 1;
			e->ScoreMultiple = 1;
			ent.Set<iw::Model>(*Asset->Load<iw::Model>("Tetrahedron"));

			break;
		}
	}

	c->SetCol(s);
	c->SetTrans(t);
	c->SetIsStatic(false);
	
	c->SetOnCollision([&](iw::Manifold& man, float dt) {
		iw::Entity enemyEntity, otherEntity;
		if (GetEntitiesFromManifold<Enemy>(man, enemyEntity, otherEntity)) {
			return;
		}

		Enemy* enemy = enemyEntity .Find<Enemy>();
		
		float score = 1000;

		if (otherEntity.Has<Player>()) {
			Player* player = otherEntity.Find<Player>();

			if (   player->Transition
				|| player->Timer <= 0.0f) // Exit if player isn't attacking
			{
				return;
			}

			score = floor(5 * (1.0f - player->Timer / player->DashTime)) * 200 + 200;
		}

		else if (otherEntity.Has<Bullet>()) {
			Bullet* bullet= otherEntity.Find<Bullet>();

			if (bullet->Type != REVERSED) { // Exit if bullet isn't reversed
				return;
			}
		}

		else {
			return; // exit if not player/bullet
		}

		Space->QueueChange(&enemy->JustHit, true);
		Space->QueueChange(&enemy->Health, enemy->Health - 1);

		if (enemyEntity.Has<EnemyBoss>()) {
			Audio->AsStudio()->CreateInstance("User/enemyDeath");
		}

		if (enemy->Health > 1) { // Exit if enemy has more health
			return;
		}

		iw::Transform* transform = enemyEntity.Find<iw::Transform>();

		Audio->AsStudio()->CreateInstance("User/enemyDeath");

		Bus->push<SpawnEnemyDeath>(transform->Position, transform->Parent());
		Bus->push<GiveScoreEvent>(transform->Position, score * enemy->ScoreMultiple);

		if (!enemyEntity.Has<EnemyBoss>()) {
			transform->SetParent(nullptr);
			Space->QueueEntity(enemyEntity.Handle, iw::func_Destroy);
		}
	});

	return t;
}
