#pragma once

#include "Components/Bullet.h"
#include "iw/engine/System.h"
#include "iw/physics/Dynamics/Rigidbody.h"
#include "iw/common/Components/Transform.h"

class BulletSystem
	: public IW::System<IW::Transform, IW::Rigidbody, Bullet>
{
public:
	struct Components {
		IW::Transform* Transform;
		IW::Rigidbody* Rigidbody;
		Bullet* Bullet;
	};

public:
	BulletSystem();

	void Update(
		IW::EntityComponentArray& view) override;
};

class BulletCollisionSystem
	: public IW::System<Bullet, IW::CollisionEvent>
{
public:
	struct Components {
		Bullet* Bullet;
		IW::CollisionEvent Collision;
	};

public:
	BulletCollisionSystem();

	void Update(
		IW::EntityComponentArray& view) override;
};
