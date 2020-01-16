#pragma once

#include "Components/Enemy.h"
#include "iw/engine/System.h"
#include "iw/common/Components/Transform.h"
#include "iw/engine/Components/Model.h"

class EnemySystem
	: public IW::System<IW::Transform, Enemy>
{
public:
	struct Components {
		IW::Transform* Transform;
		Enemy* Enemy;
	};
private:
	iw::ref<IW::Model> BulletModel;

public:
	EnemySystem(
		iw::ref<IW::Model> bulletModel);

	void Update(
		IW::EntityComponentArray& view) override;

	bool On(
		IW::CollisionEvent& e) override;
};
