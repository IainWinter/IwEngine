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

	~BulletSystem();

	//void Sync();

	void Update(
		IW::EntityComponentArray& view) override;

	bool On(
		IW::CollisionEvent& e) override;
};
