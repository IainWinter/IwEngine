#pragma once

#include "Components/Enemy.h"
#include "iw/engine/System.h"
#include "iw/common/Components/Transform.h"
#include "iw/graphics/Model.h"

class EnemySystem
	: public iw::System<iw::Transform, Enemy>
{
public:
	struct Components {
		iw::Transform* Transform;
		Enemy* Enemy;
	};
private:
	int m_enemyCount;
	float m_levelResetTimer;

	iw::Entity& player;
	iw::Prefab& m_bullet;

public:
	EnemySystem(
		iw::Entity& player,
		iw::Prefab& prefab);

	void Update(
		iw::EntityComponentArray& view) override;

	bool On(iw::ActionEvent& e) override;
private:
	iw::Transform* SpawnBullet(
		BulletType type,
		float speed,
		iw::vector3 position,
		iw::quaternion rot);

	iw::Transform* SpawnEnemy(
		Enemy prefab,
		iw::vector3 position,
		iw::vector3 velocity);
};
