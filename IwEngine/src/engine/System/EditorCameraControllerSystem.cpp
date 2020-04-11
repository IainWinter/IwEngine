#include "iw/engine/Systems/EditorCameraControllerSystem.h"
#include "iw/input/Devices/Keyboard.h"
#include "iw/engine/Time.h"
#include <imgui/imgui.h>


namespace iw {
	EditorCameraControllerSystem::EditorCameraControllerSystem()
		: System("Editor Camera Controller")
		, speed(10)
	{}

	void EditorCameraControllerSystem::Update(
		EntityComponentArray& eca)
	{
		for (auto entity : eca) {
			auto [t, c] = entity.Components.Tie<Components>();

			if (movement != 0) {
				if (movement.x != 0) {
					iw::vector3 rot = -t->Right();
					t->Position += rot * movement.x * speed * Time::DeltaTime();
				}

				if (movement.y != 0) {
					t->Position += iw::vector3::unit_y * movement.y * speed * Time::DeltaTime();
				}

				if (movement.z != 0) {
					iw::vector3 forward = t->Forward();
					forward.y = 0;
					forward.normalize();

					if (forward.length_squared() == 0) {
						forward = t->Right().cross(iw::vector3::unit_y);
					}

					t->Position += forward * movement.z * speed * Time::DeltaTime();
				}
			}
			
			if (rotation != 0) {
				iw::quaternion deltaP = iw::quaternion::from_axis_angle(t->Right(), rotation.x);
				iw::quaternion deltaY = iw::quaternion::from_axis_angle(-iw::vector3::unit_y, rotation.y);

				t->Rotation = deltaP * t->Rotation;
				t->Rotation = deltaY * t->Rotation;
			}

			rotation = 0;
		}
	}

	void EditorCameraControllerSystem::OnPush() {
		movement.y += Keyboard::KeyDown(SHIFT) ? -1 : 0;
		movement.y += Keyboard::KeyDown(SPACE) ?  1 : 0;
		
		movement.x += Keyboard::KeyDown(A) ? -1 : 0;
		movement.x += Keyboard::KeyDown(D) ?  1 : 0;
		
		movement.z += Keyboard::KeyDown(W) ?  1 : 0;
		movement.z += Keyboard::KeyDown(S) ? -1 : 0;
	}

	void EditorCameraControllerSystem::OnPop() {
		movement = 0;
	}

	bool EditorCameraControllerSystem::On(
		KeyEvent& e)
	{
		switch (e.Button) {
			case iw::val(SHIFT): movement.y -= e.State ? 1 : -1; break;
			case iw::val(SPACE): movement.y += e.State ? 1 : -1; break;

			case iw::val(A):     movement.x -= e.State ? 1 : -1; break;
			case iw::val(D):     movement.x += e.State ? 1 : -1; break;

			case iw::val(W):     movement.z += e.State ? 1 : -1; break;
			case iw::val(S):     movement.z -= e.State ? 1 : -1; break;
		}

		return false;
	}

	bool EditorCameraControllerSystem::On(
		MouseMovedEvent& e)
	{
		if (e.Device == DeviceType::RAW_MOUSE) {
			rotation.x = e.DeltaY * Time::DeltaTime();
			rotation.y = e.DeltaX * Time::DeltaTime();
		}

		return false;
	}

	bool EditorCameraControllerSystem::On(
		MouseButtonEvent& e)
	{
		if (e.Device == DeviceType::RAW_MOUSE && e.Button == RMOUSE) {
			speed = e.State ? 100 : 10;
		}

		return false;
	}
}
