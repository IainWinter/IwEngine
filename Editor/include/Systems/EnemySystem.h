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
	iw::ref<iw::Model> m_bulletModel;
	int m_enemyCount;
	float m_levelResetTimer;

public:
	EnemySystem(
		iw::ref<iw::Model> bulletModel);

	void Update(
		iw::EntityComponentArray& view) override;

	bool On(
		iw::CollisionEvent& e) override;
};
