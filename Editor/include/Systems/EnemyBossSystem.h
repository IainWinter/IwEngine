#pragma once

#include "iw/engine/System.h"
#include "Systems/EnemySystem.h"
#include <vector>

class EnemyBossSystem
	: public iw::SystemBase
{
private:
	using func_Action    = void(EnemyBossSystem::*)(iw::Transform*, Enemy*);
	using func_Condition = bool(EnemyBossSystem::*)(iw::Transform*, Enemy*);

	iw::Entity& m_player;
	const EnemySystem* m_enemySystem;

	std::vector<func_Action> m_actions;
	std::vector<func_Condition> m_conditions;

	const Bullet m_seekBullet = {
		BulletType::SEEK,
		8.0f
	};

	const Bullet m_orbitBullet = {
		BulletType::ORBIT,
		8.0f
	};

	int m_musicInstance;

public:
	EnemyBossSystem(
		iw::Entity& player,
		const EnemySystem* enemySystem);

	int Initialize() override;
	void Update() override;

private:
	void action_forest_spin       (iw::Transform* transform, Enemy* enemy);
	void action_forest_wave       (iw::Transform* transform, Enemy* enemy);
	void action_forest_wave_gap   (iw::Transform* transform, Enemy* enemy);
	void action_forest_enemy_fling(iw::Transform* transform, Enemy* enemy);

	void action_canyon_front_at_player(iw::Transform* transform, Enemy* enemy);
	void action_canyon_side_seek      (iw::Transform* transform, Enemy* enemy);
	void action_canyon_back_orbit     (iw::Transform* transform, Enemy* enemy);

	bool condition_enemy_count_or_just_hit(iw::Transform* transform, Enemy* enemy);
	bool condition_enemy_health_2         (iw::Transform* transform, Enemy* enemy);
	bool condition_enemy_health_1         (iw::Transform* transform, Enemy* enemy);
};
