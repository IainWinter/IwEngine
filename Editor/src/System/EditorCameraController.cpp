#include "Systems/EditorCameraController.h"
#include "Events/ActionEvents.h"
#include "iw/engine/Time.h"
#include <imgui/imgui.h>

namespace IW {
	EditorCameraController::EditorCameraController()
		: System("Editor Camera Controller")
		, speed(10)
		, movement(0.00001f)
	{}

	void EditorCameraController::Update(
		EntityComponentArray& eca)
	{
		for (auto entity : eca) {
			auto [t, c] = entity.Components.Tie<Components>();

			if (movement != 0) {
				if (movement.x != 0) {
					t->Position += -t->Right() * movement.x * speed * Time::DeltaTime();
				}

				if (movement.y != 0) {
					t->Position += iw::vector3::unit_y * movement.y * speed * Time::DeltaTime();
				}

				if (movement.z != 0) {
					t->Position += -t->Forward() * movement.z * speed * Time::DeltaTime();
				}

				c->Camera->Position = t->Position;
			}

			if (rotation != 0) {
				iw::quaternion deltaP = iw::quaternion::from_axis_angle(-t->Right(), rotation.x);
				iw::quaternion deltaY = iw::quaternion::from_axis_angle(iw::vector3::unit_y, rotation.y);

				t->Rotation *= deltaP * deltaY;

				c->Camera->Rotation = t->Rotation;

				rotation = 0;
			}
		}
	}

	bool EditorCameraController::On(
		ActionEvent& e)
	{
		switch (e.Action) {
			case iw::val(Actions::JUMP):    movement.y += e.as<ToggleEvent>().Active ?  1 : -1; break;
			case iw::val(Actions::RIGHT):   movement.x += e.as<ToggleEvent>().Active ?  1 : -1; break;
			case iw::val(Actions::FORWARD): movement.z += e.as<ToggleEvent>().Active ? -1 :  1; break;
		}

		return false;
	}

	bool EditorCameraController::On(
		MouseMovedEvent& e)
	{
		if (e.Device == DeviceType::RAW_MOUSE) {
			rotation.x = e.DeltaY * 0.0005f;
			rotation.y = e.DeltaX * 0.0005f; // sens?
		}

		return false;
	}

	bool EditorCameraController::On(
		MouseButtonEvent& e)
	{
		if (e.Device == DeviceType::RAW_MOUSE && e.Button == RMOUSE) {
			speed = e.State ? 50 : 10;
		}

		LOG_INFO << speed;

		return false;
	}
}
