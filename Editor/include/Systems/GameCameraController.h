#pragma once

#include "iw/engine/System.h"
#include "iw/engine/Components/CameraController.h"

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
	iw::Entity& player;

	iw::vector3 center;
	float speed;

public:
	GameCameraController(
		iw::Entity& player);

	void SetLocked(
		bool locked)
	{
		locked = true;
	}

	void Update(
		iw::EntityComponentArray& eca) override;

	bool On(
		iw::ActionEvent& e) override;
};
