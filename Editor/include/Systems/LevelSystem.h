#pragma once

#include "iw/engine/System.h"
#include "iw/common/Components/Transform.h"
#include "Components/Level.h"

class LevelSystem
	: public iw::System<>
{
private:
	std::string levelName;
	Level level;
	iw::Entity door;

public:
	LevelSystem();

	int Initialize() override;

	bool On(
		iw::CollisionEvent& event);

	bool On(
		iw::ActionEvent& event);

	void LoadLevel(
		std::string name);

	void LoadTree(
		std::string name,
		iw::Transform transform);

	void LoadFloor(
		std::string name,
		iw::Transform transform);
};
