#pragma once

#include "Components/Bullet.h"
#include "iw/engine/System.h"
#include "iw/data/Components/Transform.h"

class BulletSystem
	: public IW::System<IW::Transform, Bullet>
{
public:
	BulletSystem(
		IW::Space& space,
		IW::Graphics::Renderer& renderer);

	~BulletSystem();

	//void Sync();

	void Update(
		IW::EntityComponentArray& view) override;
};
