#pragma once

#include "Components/Bullet.h"
#include "iw/engine/System.h"
#include "iw/engine/Components/Transform.h"

class BulletSystem
	: public IwEngine::System<IwEngine::Transform, Bullet>
{
public:
	BulletSystem(
		IwEntity::Space& space);

	~BulletSystem();

	void Update(
		IwEntity::View<IwEngine::Transform, Bullet>& view) override;
};
