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
	bool m_follow, m_locked;

	iw::vector3 m_previous, m_center;
	float m_speed, m_y;

	iw::Entity& m_target;

public:
	GameCameraController(
		iw::Entity& target);

	void SetLocked(
		bool locked)
	{
		locked = true;
	}

	void Update(
		iw::EntityComponentArray& eca) override;

	bool On(
		iw::ActionEvent& e) override;
private:
	void SetTarget(
		std::string_view levelName);
};
