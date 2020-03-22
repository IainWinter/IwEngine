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
	bool follow;
	iw::Entity& player;

public:
	GameCameraController(
		iw::Entity& player);

	void Update(
		iw::EntityComponentArray& eca) override;

	bool On(
		iw::ActionEvent& e) override;
};
