#pragma once

#include "Components/EnemyDeathCircle.h"
#include "iw/engine/System.h"
#include "iw/common/Components/Transform.h"
#include "iw/graphics/Model.h"

class EnemyDeathCircleSystem
	: public iw::System<iw::Transform, iw::Model, iw::CollisionObject, EnemyDeathCircle>
{
public:
	struct Components {
		iw::Transform* Transform;
		iw::Model* Model;
		iw::CollisionObject* Object;
		EnemyDeathCircle* DeathCircle;
	};
private:
	iw::ref<iw::Model> m_deathCircleModel;
	EnemyDeathCircle m_prefab;

public:
	EnemyDeathCircleSystem();

	int Initialize() override;

	void Update(
		iw::EntityComponentArray& view) override;

	bool On(iw::ActionEvent&          e) override;
private:
	iw::Transform* SpawnDeathCircle(
		EnemyDeathCircle prefab,
		iw::vector3 position);
};
