#pragma once

#include "iw/engine/System.h"
#include "Components/Bullet.h"
#include "iw/physics/Collision/CapsuleCollider.h"

class SpecialBarrierSystem
	: public iw::SystemBase
{
private:
	iw::Entity& player;
	bool dashThrough;

public:
	SpecialBarrierSystem(
		iw::Entity& player);

	void collide(iw::Manifold& man, iw::scalar dt);

	void Update() override;

	bool On(iw::ActionEvent& e) override;
private:
	iw::Transform* SpawnSpecialBarrier(
		iw::CapsuleCollider capsule);
};
