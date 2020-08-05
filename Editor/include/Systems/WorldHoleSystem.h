#pragma once

#include "iw/engine/System.h"

class WorldHoleSystem
	: public iw::SystemBase
{
private:
	iw::Entity& currentLevel;
	iw::Prefab holePrefab;

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
		bool crumble,
		std::string caveLevel);
};
