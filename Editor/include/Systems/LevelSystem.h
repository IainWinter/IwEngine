#pragma once

#include "iw/engine/System.h"
#include "iw/common/Components/Transform.h"
#include "Components/LevelDoor.h"
#include "Components/Level.h"
#include "iw/graphics/Model.h"

#include "iw/engine/Events/Seq/EventSequence.h"

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

	iw::Entity levelDoor;
	iw::Entity nextLevelDoor;
	iw::Color openColor;
	iw::Color closedColor;

	iw::EventSequence sequence; // For cut scenes, prob shouldnt be here
	iw::Entity firstEnemy;
	iw::Entity otherGuy;

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
