#pragma once

#include "Components/Bullet.h"
#include "iw/engine/System.h"
#include "iw/common/Components/Transform.h"

class BulletSystem
	: public IW::System<IW::Transform, Bullet>
{
public:
	BulletSystem();

	~BulletSystem();

	//void Sync();

	void Update(
		IW::EntityComponentArray& view) override;
};
