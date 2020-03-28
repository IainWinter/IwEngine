#pragma once

#include "iw/engine/System.h"
#include "iw/common/Components/Transform.h"
#include "Components/LevelDoor.h"
#include "Components/Level.h"
#include "iw/graphics/Model.h"

class LevelSystem
	: public iw::System<iw::Transform, iw::Model, LevelDoor>
{
public:
	struct Components {
		iw::Transform* Transform;
		iw::Model* Model;
		LevelDoor* Door;
	};
private:
	std::string currentLevelName;
	Level currentLevel;

	iw::Entity& playerEntity;

	iw::Entity levelEntity;
	iw::Entity nextLevelEntity;
	bool transition;

	iw::Entity currentDoor;
	iw::Color openColor;
	iw::Color closedColor;

public:
	LevelSystem(
		iw::Entity& player);

	int Initialize() override;

	void Update(
		iw::EntityComponentArray& view) override;

	bool On(
		iw::CollisionEvent& event);

	bool On(
		iw::ActionEvent& event);

	iw::Entity LoadLevel(
		std::string name);

	void LoadTree(
		std::string name,
		iw::Transform transform);

	iw::Entity LoadFloor(
		std::string name,
		iw::Transform transform);
private:
	void DestroyAll(
		iw::Transform* transform);
};