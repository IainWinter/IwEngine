#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/physics/Dynamics/Rigidbody.h"
#include "iw/engine/Components/CameraController.h"

struct Car {

};

class CarControllerSystem
	: public iw::System<iw::Transform, iw::Rigidbody, Car>
{
public:
	struct Components {
		iw::Transform* Transform;
		iw::Rigidbody* Rigidbody;
		Car* Car;
	};
private:
	iw::vector3 movement;
	float rotation;
	float speed;

public:
	IWENGINE_API
	CarControllerSystem();

	//int Initialize() override;

	IWENGINE_API
	void Update(
		iw::EntityComponentArray& eca) override;

	void OnPush() override;
	void OnPop() override;

	IWENGINE_API
	bool On(iw::KeyEvent& e) override;

	IWENGINE_API
	bool On(iw::MouseMovedEvent& e) override;

	IWENGINE_API
	bool On(iw::MouseButtonEvent& e) override;
};
