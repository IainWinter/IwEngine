#include "Systems/EnemyBossSystem.h"

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

#include "Systems/EnemySystem.h"

EnemyBossSystem::EnemyBossSystem(
	iw::Entity& player,
	const EnemySystem* enemySystem,
	GameSaveState* saveState)
	: iw::SystemBase("Enemy Boss")
	, m_player(player)
	, m_enemySystem(enemySystem)
	, m_saveState(saveState)
	, m_musicInstance(-1)
{}

int EnemyBossSystem::Initialize() {
	m_actions.push_back(&EnemyBossSystem::action_forest_spin);
	m_actions.push_back(&EnemyBossSystem::action_forest_wave);
	m_actions.push_back(&EnemyBossSystem::action_forest_wave_gap);
	m_actions.push_back(&EnemyBossSystem::action_forest_enemy_fling);

	m_actions.push_back(&EnemyBossSystem::action_canyon_front_at_player);
	m_actions.push_back(&EnemyBossSystem::action_canyon_side_seek);
	m_actions.push_back(&EnemyBossSystem::action_canyon_back_orbit);

	m_actions.push_back(&EnemyBossSystem::action_any_move_random);

	m_conditions.push_back(&EnemyBossSystem::condition_enemy_count_or_just_hit);

	return 0;
}

void EnemyBossSystem::Update() {
	auto bosses = Space->Query<iw::CollisionObject, Enemy, EnemyBoss>();

	bosses.Each([&](
		iw::EntityHandle entity,
		iw::CollisionObject*   object,
		Enemy*     enemy,
		EnemyBoss* boss)
	{
		iw::Transform* transform = Space->FindComponent<iw::Transform>(entity);

		if (boss->JustSpawned) {
			if (m_musicInstance != -1) {
				Audio->AsStudio()->StopInstance(m_musicInstance);
				Audio->AsStudio()->StartInstance(m_musicInstance);
			}

			else {
				m_musicInstance = Audio->AsStudio()->CreateInstance("Music/boss", false);
			}

			boss->JustSpawned = false;
			boss->CurrentAction = boss->FirstAction;
		}

		Audio->AsStudio()->SetInstanceParameter(m_musicInstance, "BossHealth", enemy->Health);

		if (   enemy->Health == 0
			&& enemy->Type == EnemyType::MINI_BOSS_CANYON)
		{
			m_saveState->Canyon03BossKilled = true;

			transform->SetParent(nullptr);
			Space->QueueEntity(entity, iw::func_Destroy);
		}

		if (enemy->ChargeTime > enemy->Timer) {
			return;
		}

		if (boss->CurrentAction == -1) {
			int index = -1;

			for (int i = 0; i < boss->Actions.size(); i++) {
				Action& action = boss->Actions.at(i);
				
				if (action.EveryTime == 1) {
					index = i;
					action.EveryTime = 0;
					break;
				}

				else if (action.EveryTime == 0) {
					action.EveryTime = 1;
				}

				else if (action.Condition != -1) {
					bool pickme = std::invoke(m_conditions.at(action.Condition), this, transform, enemy, boss);
					if (pickme) {
						index = i;
						break;
					}
				}
			}

			if (index == -1) {
				do {
					index = iw::randi(boss->Actions.size() - 1);
				} while (boss->Actions.at(index).EveryTime != -1);
			}

			boss->CurrentAction = index;
		}
		
		Action& action = boss->Actions.at(boss->CurrentAction); 

		if (action.Time > enemy->Timer - enemy->ChargeTime) {
			if (boss->ActionTimer == 0.0f) {
				std::invoke(m_actions.at(action.Index), this, transform, enemy, boss);
			}

			boss->ActionTimer += iw::Time::DeltaTimeScaled();

			if (boss->ActionTimer > action.Delay) {
				boss->ActionTimer = 0.0f;
			}
		}

		else if (action.Time < enemy->Timer - enemy->ChargeTime) {
			enemy->Timer = 0;
			boss->ActionTimer = 0;
			boss->CurrentAction = -1;
		}

		object->SetTrans(transform);
	});
}

bool EnemyBossSystem::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::START_LEVEL): {
			StartLevelEvent& event = e.as<StartLevelEvent>();
			if (   event.LevelName == "levels/canyon/canyon06.json"
				&& !m_saveState->Canyon03BossKilled)
			{
				iw::Entity entity = Space->CreateEntity<
					iw::Transform, 
					iw::CollisionObject, 
					iw::SphereCollider>();

				iw::Transform*       t = entity.Set<iw::Transform>(iw::vector3(13, 0, -4));
				iw::SphereCollider*  c = entity.Set<iw::SphereCollider>(0, 4);
				iw::CollisionObject* o = entity.Set<iw::CollisionObject>();

				t->SetParent(event.Level);

				o->SetTrans(t);
				o->SetCol(c);
				o->SetIsTrigger(true);
				o->SetOnCollision([&](auto man, auto) {
					if (m_enemySystem && m_enemySystem->GetEnemyCount() != 0) {
						return;
					}

					iw::Entity player, me;
					if (GetEntitiesFromManifold<Player>(man, player, me)) {
						return;
					}

					Bullet bullet;
					bullet.Type = BulletType::LINE; bullet.Speed = 5; 
					bullet.Package = PackageType::NONE;

					Enemy boss;
					boss.Type = EnemyType::MINI_BOSS_CANYON; boss.Bullet = bullet;
					boss.Speed = 0.174533f; boss.FireTime = 4; boss.ChargeTime = 2; boss.Rotation = 0;
				
					Bus->push<SpawnEnemyEvent>(boss, iw::vector3(12, 9, 6), 0, 
						me.Find<iw::Transform>()->Parent());
					Bus->push<UnlockLevelDoorEvent>(false);

					me.Destroy();
				});

				Physics->AddCollisionObject(o); 
			}
			break;
		}
		case iw::val(Actions::RESET_LEVEL):
		case iw::val(Actions::AT_NEXT_LEVEL):
		case iw::val(Actions::GOTO_NEXT_LEVEL): { // not sure if next level 
			if (m_musicInstance != -1) {
				Audio->AsStudio()->StopInstance(m_musicInstance);
				Audio->AsStudio()->RemoveInstance(m_musicInstance);
				m_musicInstance = -1;
			}
			break;
		}
	}

	return false;
}

void EnemyBossSystem::action_forest_spin(
	iw::Transform* transform,
	Enemy* enemy, 
	EnemyBoss* boss)
{
	float rot = enemy->Timer * iw::Pi2 * 2;
	iw::quaternion offset = iw::quaternion::from_euler_angles(0, rot, 0);

	Bus->push<SpawnBulletEvent>(
		enemy->Bullet,
		transform->Position,
		transform->Rotation.inverted() * offset,
		transform->Parent()
	);
}

void EnemyBossSystem::action_forest_wave(
	iw::Transform* transform,
	Enemy* enemy,
	EnemyBoss* boss)
{
	float rot = iw::Pi + iw::hPi * 0.5f * cos(enemy->Timer);

	int count = roundf(iw::Pi2 / enemy->Speed);
	int dontShoot = count * 0.5f;

	for (int i = 0; i <= count; i++) {
		if (   i != dontShoot
			&& i != dontShoot - 1
			&& i != dontShoot + 1)
		{
			iw::quaternion offset = iw::quaternion::from_euler_angles(0, -rot + enemy->Speed * i, 0);

			Bus->push<SpawnBulletEvent>(
				enemy->Bullet,
				transform->Position,
				transform->Rotation.inverted() * offset,
				transform->Parent()
			);
		}
	}
}

void EnemyBossSystem::action_forest_wave_gap(
	iw::Transform* transform,
	Enemy* enemy,
	EnemyBoss* boss)
{
	int count = roundf(iw::Pi2 / enemy->Speed);
	float rot = iw::Pi + iw::hPi * 0.5f * cos(enemy->Timer);

	for (int i = 0; i < count; i++) {
		iw::quaternion offset = iw::quaternion::from_euler_angles(0, -rot + enemy->Speed * i, 0);

		Bus->push<SpawnBulletEvent>(
			enemy->Bullet,
			transform->Position,
			transform->Rotation.inverted() * offset,
			transform->Parent()
		);
	}
}

void EnemyBossSystem::action_forest_enemy_fling(
	iw::Transform* transform,
	Enemy* enemy,
	EnemyBoss* boss)
{
	float rot = iw::Time::TotalTime() * iw::Pi2 * 3;

	iw::quaternion offset = transform->Rotation.inverted()
		* iw::quaternion::from_euler_angles(0, rot, 0);

	Enemy child{};
	child.Type = EnemyType::SPIN;
	child.Bullet = enemy->Bullet;
	child.Speed = 0.2617994;
	child.FireTime = 0.120000;
	child.ChargeTime = 0.000000;
	child.HasShot = false;
	child.JustHit = false;

	iw::vector3 position = transform->Position + iw::vector3(sqrt(2), 1, 0) * offset;
	iw::vector3 velocity = transform->Forward() * offset;

	velocity *= 7.0f + iw::randf() * 2;
	velocity.y = 10.0f;

	Bus->push<SpawnEnemyEvent>(
		child,
		position,
		velocity,
		transform->Parent());
}

void EnemyBossSystem::action_canyon_front_at_player(
	iw::Transform* transform,
	Enemy* enemy,
	EnemyBoss* boss)
{
	int count = roundf(iw::hPi / enemy->Speed);
	float rot = -iw::hPi * 0.5f;

	for (int i = 0; i < count; i++) {
		iw::quaternion offset = iw::quaternion::from_euler_angles(0, rot + i * enemy->Speed, 0);

		Bus->push<SpawnBulletEvent>(
			enemy->Bullet,
			transform->Position,
			transform->Rotation.inverted() * offset,
			transform->Parent()
		);
	}

	if (enemy->Health <= 2 && iw::randf() > 0.6) {
		action_canyon_side_seek(transform, enemy, boss);
	}
}

void EnemyBossSystem::action_canyon_side_seek(
	iw::Transform* transform,
	Enemy* enemy,
	EnemyBoss* boss)
{
	int count = 4;
	float rot = iw::hPi * 0.5f;

	for (int i = 0; i < count; i++) {
		iw::quaternion offset = iw::quaternion::from_euler_angles(0, rot + i * iw::hPi / count, 0);

		Bus->push<SpawnBulletEvent>(
			m_seekBullet,
			transform->Position,
			transform->Rotation.inverted() * offset,
			transform->Parent()
		);
	}

	for (int i = 0; i < count; i++) {
		iw::quaternion offset = iw::quaternion::from_euler_angles(0, -rot - i * iw::hPi / count, 0);

		Bus->push<SpawnBulletEvent>(
			m_seekBullet,
			transform->Position,
			transform->Rotation.inverted() * offset,
			transform->Parent()
		);
	}

	if (enemy->Health == 1) {
		action_canyon_back_orbit(transform, enemy, boss);
	}
}

void EnemyBossSystem::action_canyon_back_orbit(
	iw::Transform* transform,
	Enemy* enemy,
	EnemyBoss* boss)
{
	int count = 4;
	float rot = iw::Pi + -iw::hPi * 0.5f;

	for (int i = 0; i < count; i++) {
		iw::quaternion offset = iw::quaternion::from_euler_angles(0, rot + i * iw::Pi / count, 0);

		Bus->push<SpawnBulletEvent>(
			m_orbitBullet,
			transform->Position,
			transform->Rotation.inverted() * offset,
			transform->Parent()
		);
	}
}

void EnemyBossSystem::action_any_move_random(
	iw::Transform* transform,
	Enemy* enemy,
	EnemyBoss* boss)
{
	// test if random position is not causing a collisoin
	// move there

	if (enemy->Timer - enemy->ChargeTime < iw::Time::DeltaTime()) {
		int side = boss->Target.dot(iw::vector3::unit_x) > 0 ? -1 : 1;

		iw::vector3 pos;
		do {
			pos = iw::vector3(side * 12.5f + iw::randf() * 5, 1, iw::randf() * 12);
		} while (Physics->TestCollider(iw::SphereCollider(pos, 2)));
		boss->Target = pos;
	}

	transform->Position = boss->Target;

	//float distance = iw::vector2(boss->Target.x - transform->Position.x, boss->Target.z - transform->Position.z).length_fast();
	//float actionTime = boss->Actions.at(boss->CurrentAction).Time;
	//float speed = distance / actionTime;

	//transform->Position.x = iw::lerp(transform->Position.x, boss->Target.x, iw::Time::DeltaTime() * actionTime);
	//transform->Position.z = iw::lerp(transform->Position.z, boss->Target.z, iw::Time::DeltaTime() * actionTime);
	//transform->Position.y = (-pow((enemy->Timer - enemy->ChargeTime)*2 - actionTime, 2) + pow(actionTime, 2)) * 3 + 1;
}

bool EnemyBossSystem::condition_enemy_count_or_just_hit(
	iw::Transform* transform,
	Enemy* enemy,
	EnemyBoss* boss)
{
	return m_enemySystem->GetEnemyCount() > 4 || enemy->JustHit;
}

bool EnemyBossSystem::condition_enemy_health_2(
	iw::Transform* transform,
	Enemy* enemy,
	EnemyBoss* boss)
{
	return enemy->Health == 2;
}

bool EnemyBossSystem::condition_enemy_health_1(
	iw::Transform* transform,
	Enemy* enemy,
	EnemyBoss* boss)
{
	return enemy->Health == 1;
}
