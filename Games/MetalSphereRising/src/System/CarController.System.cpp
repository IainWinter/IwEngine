#include "Systems/CarControllerSystem.h"
#include "iw/input/Devices/Keyboard.h"
#include "iw/engine/Time.h"
#include <imgui/imgui.h>

CarControllerSystem::CarControllerSystem()
	: System("Car Controller")
	, speed(166.666f)
{}

void CarControllerSystem::Update(
	iw::EntityComponentArray& eca)
{
	for (auto entity : eca) {
		auto [t, r, c] = entity.Components.Tie<Components>();

		iw::quaternion deltaY = iw::quaternion::from_axis_angle(iw::vector3::unit_y, rotation);
		t->Rotation *= deltaY;

		if (movement != 0) {
			r->ApplyForce(movement * deltaY *  50);
			
			if (r->Velocity().length() > speed) {
				r->SetVelocity(r->Velocity().normalized() * speed);
			}

			LOG_INFO << r->Velocity();
		}

		else {
			r->SetVelocity(r->Velocity() * 0.99f);
		}
	}
}

void CarControllerSystem::OnPush() {
	//movement.y += iw::Keyboard::KeyDown(iw::SHIFT) ? -1 : 0;
	//movement.y += iw::Keyboard::KeyDown(iw::SPACE) ?  1 : 0;
		
	movement.x += iw::Keyboard::KeyDown(iw::W) ? -1 : 0;
	movement.x += iw::Keyboard::KeyDown(iw::S) ?  1 : 0;
		
	movement.z += iw::Keyboard::KeyDown(iw::A) ?  1 : 0;
	movement.z += iw::Keyboard::KeyDown(iw::D) ? -1 : 0;
}

void CarControllerSystem::OnPop() {
	movement = 0;
}

bool CarControllerSystem::On(
	iw::KeyEvent& e)
{
	switch (e.Button) {
		case iw::val(iw::A):     rotation -= e.State ? 1 : -1; break;
		case iw::val(iw::D):     rotation += e.State ? 1 : -1; break;

		case iw::val(iw::W):     movement.x += e.State ? 1 : -1; break;
		case iw::val(iw::S):     movement.x -= e.State ? 1 : -1; break;
	}

	return false;
}

bool CarControllerSystem::On(
	iw::MouseMovedEvent& e)
{
	if (e.Device == iw::DeviceType::RAW_MOUSE) {
		//rotation.x = e.DeltaY * iw::Time::DeltaTime();
		//rotation.y = e.DeltaX * iw::Time::DeltaTime();
	}

	return false;
}

bool CarControllerSystem::On(
	iw::MouseButtonEvent& e)
{
	return false;
}
