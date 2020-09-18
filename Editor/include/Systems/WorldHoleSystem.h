#pragma once

#include "iw/engine/System.h"
#include "Components/Bullet.h"

class WorldHoleSystem
	: public iw::SystemBase
{
private:
	iw::Entity& currentLevel;
	iw::Prefab holePrefab;

	bool m_active;

	const Bullet m_seekBullet = {
		BulletType::SEEK,
		8.0f
	};

	const Bullet m_orbitBullet = {
		BulletType::ORBIT,
		8.0f
	};

public:
	WorldHoleSystem(
		iw::Entity& currentLevel);

	void collide(iw::Manifold& man, iw::scalar dt);

	int Initialize()   override;
	void Update() override;

	bool On(iw::ActionEvent& e) override;
private:
	void SpawnHole(
		iw::vector3 position,
		iw::vector3 scale,
		bool crumble,
		std::string caveLevel);
};
