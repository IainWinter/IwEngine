#pragma once

#include "iw/engine/System.h"
#include "iw/engine/Components/CameraController.h"
#include "iw/graphics/Scene.h"

class GameCameraController
	: public iw::System<iw::Transform, iw::CameraController>
{
public:
	struct Components {
		iw::Transform*        Transform;
		iw::CameraController* Controller;
	};
private:
	bool locked;
	bool follow;
	bool transitionToCenter;

	iw::vector3 center;
	float speed;

	iw::Entity& m_target;
	iw::Scene* scene;

public:
	GameCameraController(
		iw::Entity& target,
		iw::Scene* scene);

	void SetLocked(
		bool locked)
	{
		locked = true;
	}

	void Update(
		iw::EntityComponentArray& eca) override;

	bool On(
		iw::ActionEvent& e) override;

	void SetTarget(
		std::string_view levelName);
};
