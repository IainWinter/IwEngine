#pragma once

#include "iw/engine/System.h"

class BulletSystem
	: public iw::SystemBase
{
private:
	iw::Entity& player;
	iw::Prefab bulletPrefab;

	iw::ref<iw::Material> normalMat;
	iw::ref<iw::Material> orbitMat;
	iw::ref<iw::Material> seekMat;

public:
	BulletSystem(
		iw::Entity& player);

	iw::Prefab& GetBulletPrefab() { return bulletPrefab; }

	void collide(iw::Manifold& man, iw::scalar dt);

	int Initialize()   override;
	void FixedUpdate() override;
};
