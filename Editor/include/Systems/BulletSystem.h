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

public:
	BulletSystem();

	void FixedUpdate(
		iw::EntityComponentArray& view) override;

	bool On(iw::CollisionEvent& event) override;
};
