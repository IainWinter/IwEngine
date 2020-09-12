#pragma once

#include "iw/engine/System.h"
#include "Components/Bullet.h"
#include "iw/physics/Collision/CapsuleCollider.h"

class SpecialBarrierSystem
	: public iw::SystemBase
{
private:
	iw::Entity& currentLevel;
	iw::Entity& player;

	bool dashThrough;

public:
	SpecialBarrierSystem(
		iw::Entity& currentLevel,
		iw::Entity& player);

	void collide(iw::Manifold& man, iw::scalar dt);

	int Initialize()   override;
	void Update() override;

	bool On(iw::ActionEvent& e) override;
private:
	iw::Transform* SpawnSpecialBarrier(
		iw::CapsuleCollider capsule);
};
