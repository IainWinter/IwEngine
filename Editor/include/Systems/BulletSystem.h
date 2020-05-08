#pragma once

#include "Components/Bullet.h"
#include "iw/engine/System.h"
#include "iw/physics/Dynamics/Rigidbody.h"
#include "iw/common/Components/Transform.h"

class BulletSystem
	: public iw::System<iw::Transform, iw::Rigidbody, Bullet>
{
public:
	struct Components {
		iw::Transform* Transform;
		iw::Rigidbody* Rigidbody;
		Bullet* Bullet;
	};
private:
	struct BulletComponents {
		iw::Transform* Transform;
		Bullet* Bullet;
	};

	iw::Entity& player;

public:
	BulletSystem(
		iw::Entity& player);

	void FixedUpdate(
		iw::EntityComponentArray& view) override;
};
