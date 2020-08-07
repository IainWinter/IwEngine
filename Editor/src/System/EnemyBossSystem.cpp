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

EnemyBossSystem::EnemyBossSystem(
	iw::Entity& player)
	: iw::SystemBase("Enemy Boss")
	, m_player(player)
{}

int EnemyBossSystem::Initialize() {
	m_actions.push_back(&EnemyBossSystem::action_forest_spin);
	m_actions.push_back(&EnemyBossSystem::action_forest_wave);

	return 0;
}

void EnemyBossSystem::Update() {
	auto bosses = Space->Query<iw::Transform, Enemy, EnemyBoss>();

	bosses.Each([&](
		iw::EntityHandle entity,
		iw::Transform*   transform,
		Enemy*           enemy,
		EnemyBoss*       boss)
	{
		if (boss->CurrentAction == -1) {
			boss->CurrentAction = iw::randi(boss->Actions.size() - 1);
		}
		
		Action& action = boss->Actions.at(boss->CurrentAction);

		if (action.Time > enemy->Timer) {
			std::invoke(m_actions.at(action.Index), this, transform, enemy->Timer, boss->ActionTimer);
		}

		else if (action.Time + boss->ActionDelay < enemy->Timer) {
			enemy->Timer = 0;
			boss->ActionTimer = 0;
			boss->CurrentAction = -1;
		}
	});
}

void EnemyBossSystem::action_forest_spin(
	iw::Transform* transform,
	float timer,
	float& actionTimer)
{
	if (actionTimer == 0.0f) {
		float rot = timer * iw::Pi2 * 2;
		iw::quaternion offset = iw::quaternion::from_euler_angles(0, rot, 0);

		Bus->push<SpawnBulletEvent>(
			m_lineBullet,
			transform->Position,
			transform->Rotation.inverted() * offset,
			transform->Parent()
		);
	}

	actionTimer += iw::Time::DeltaTimeScaled();

	if (actionTimer > 0.02f) {
		actionTimer = 0.0f;
	}
}

void EnemyBossSystem::action_forest_wave(
	iw::Transform* transform,
	float timer,
	float& actionTimer)
{
	if (actionTimer == 0.0f) {
		float rot = iw::Pi + iw::hPi * 0.5f * cos(timer);

		int count = roundf(iw::Pi2 / 0.174533);
		int dontShoot = count * 0.5f;

		for (int i = 0; i <= count; i++) {
			if (   i != dontShoot
				&& i != dontShoot - 1
				&& i != dontShoot + 1)
			{
				iw::quaternion offset = iw::quaternion::from_euler_angles(0, -rot + 0.174533f * i, 0);

				Bus->push<SpawnBulletEvent>(
					m_lineBullet,
					transform->Position,
					transform->Rotation.inverted() * offset,
					transform->Parent()
				);
			}
		}
	}

	actionTimer += iw::Time::DeltaTimeScaled();

	if (actionTimer > 0.2f) {
		actionTimer = 0.0f;
	}
}
