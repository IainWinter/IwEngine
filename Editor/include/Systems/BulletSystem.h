#pragma once

#include "iw/engine/System.h"
#include "Components/Bullet.h"

class BulletSystem
	: public iw::SystemBase
{
private:
	iw::Entity& player;
	iw::Prefab bulletPrefab;

	iw::ref<iw::Material> normalMat;
	iw::ref<iw::Material> orbitMat;
	iw::ref<iw::Material> seekMat;

	iw::ref<iw::Material> reverseMat;

public:
	BulletSystem(
		iw::Entity& player);

	iw::Prefab& GetBulletPrefab() { return bulletPrefab; }

	int Initialize()   override;
	void FixedUpdate() override;

	bool On(iw::ActionEvent& e) override;

private:
	iw::Transform* SpawnBullet(
		Bullet enemyBullet,
		iw::vector3 position,
		iw::quaternion rot,
		float dist = 2);

	void SpawnBulletsFromPackage(
		Bullet* parentBullet,
		iw::Transform* transform,
		BulletPackage* package);
};
