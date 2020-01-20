#include "Systems/EditorCameraController.h"
#include "Events/ActionEvents.h"
#include "iw/engine/Time.h"
#include <imgui/imgui.h>

namespace IW {
	EditorCameraController::EditorCameraController()
		: System("Editor Camera Controller")
		, speed(10)
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
					iw::vector3 forward = t->Forward();
					forward.y = 0;
					forward.normalize();

					t->Position += -forward * movement.z * speed * Time::DeltaTime();
				}

			}
			
			if (rotation != 0) {
				iw::quaternion deltaP = iw::quaternion::from_axis_angle(-t->Right(), rotation.x);
				iw::quaternion deltaY = iw::quaternion::from_axis_angle(iw::vector3::unit_y, rotation.y);

				t->Rotation *= deltaP * deltaY;
			}

			rotation = 0;
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
			rotation.x = e.DeltaY * Time::DeltaTime() * .4f;
			rotation.y = e.DeltaX * Time::DeltaTime() * .4f;
		}

		return false;
	}

	bool EditorCameraController::On(
		MouseButtonEvent& e)
	{
		if (e.Device == DeviceType::RAW_MOUSE && e.Button == RMOUSE) {
			speed = e.State ? 50 : 10;
		}

		return false;
	}
}
