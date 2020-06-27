#pragma once

#include "iw/engine/System.h"

class BulletSystem
	: public iw::SystemBase
{
private:
	iw::Entity& player;
	iw::Prefab bulletPrefab;

public:
	BulletSystem(
		iw::Entity& player);

	~BulletSystem() {}

	iw::Prefab& GetBulletPrefab() { return bulletPrefab; }

	int Initialize()   override;
	void FixedUpdate() override;
};
