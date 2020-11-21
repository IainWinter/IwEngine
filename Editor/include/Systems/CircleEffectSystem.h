#pragma once

#include "Components/EnemyDeathCircle.h"
#include "iw/engine/System.h"
#include "iw/common/Components/Transform.h"
#include "iw/graphics/Model.h"

class CircleEffectSystem
	: public iw::SystemBase
{
private:
	struct Components {
		iw::Transform* Transform;
		iw::Model* Model;
		iw::CollisionObject* Object;
		EffectCircle* EffectCircle;
	};

	iw::Prefab m_effectCirclePrefab;

	EffectCircle m_deathCircle;
	EffectCircle m_bulletReversal;

	bool m_charged;

public:
	CircleEffectSystem();

	int Initialize() override;
	void Update() override;

	bool On(iw::ActionEvent& e) override;
private:
	iw::Transform* SpawnEffectCircle(
		EffectCircle circle,
		iw::vector3 position);
};
