#pragma once

#include "Components/Enemy.h"
#include "iw/engine/System.h"
#include "iw/engine/Components/Transform.h"

class EnemySystem
	: public IwEngine::System<IwEngine::Transform, Enemy>
{
public:
	EnemySystem(
		IwEntity::Space& space);

	~EnemySystem();

	void Update(
		View& view) override;
};
