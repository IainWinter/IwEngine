#pragma once

#include "iw/engine/System.h"
#include "Components/Bullet.h"
#include <vector>

class EnemyBossSystem
	: public iw::SystemBase
{
private:
	using func_Action = void(EnemyBossSystem::*)(iw::Transform*, float, float&);

	iw::Entity& m_player;
	std::vector<func_Action> m_actions;

	const Bullet m_lineBullet = {
		BulletType::LINE,
		5.0f
	};

public:
	EnemyBossSystem(
		iw::Entity& player);

	int Initialize() override;
	void Update() override;

private:
	void action_forest_spin(
		iw::Transform* transform,
		float timer,
		float& actionTimer);

	void action_forest_wave(
		iw::Transform* transform,
		float timer,
		float& actionTimer);
};
