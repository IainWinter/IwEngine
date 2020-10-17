#pragma once

#include "iw/engine/System.h"
#include "Components/Bullet.h"

class WorldHoleSystem
	: public iw::SystemBase
{
private:
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
	WorldHoleSystem();

	int Initialize() override;
	void Update() override;

	bool On(iw::ActionEvent& e) override;
private:
	iw::Transform* SpawnHole(
		iw::vector3 position,
		iw::vector3 scale,
		bool crumble,
		std::string caveLevel);
};
