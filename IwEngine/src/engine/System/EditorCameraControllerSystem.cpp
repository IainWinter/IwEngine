#include "iw/engine/Systems/EditorCameraControllerSystem.h"
#include "iw/input/Devices/Keyboard.h"
#include "iw/engine/Time.h"
#include <imgui/imgui.h>

namespace iw {
	EditorCameraControllerSystem::EditorCameraControllerSystem()
		: SystemBase("Editor Camera Controller")
		, speed(5.0f)
		, camera(new PerspectiveCamera(1.7f, 1.777f, 0.01f, 1000.0f))
	{}

	int EditorCameraControllerSystem::Initialize() {
		iw::Entity entity = Space->CreateEntity<Transform, EditorCameraController>();

		cameraTransform = entity.Set<Transform>(vector3(0, 10, -30));
		                  entity.Set<EditorCameraController>(camera);

		camera->SetTrans(cameraTransform);

		return 0;
	}

	void EditorCameraControllerSystem::Update()
	{
		if (!active) return;

		if (movement != 0) {
			iw::vector3 delta;

			if (movement.x != 0) {
				delta += -cameraTransform->Right() * movement.x * speed * Time::DeltaTime();
			}

			if (movement.y != 0) {
				delta += iw::vector3::unit_y * movement.y * speed * Time::DeltaTime();
			}

			if (movement.z != 0) {
				iw::vector3 forward = cameraTransform->Forward();
				forward.y = 0;
				forward.normalize();

				if (forward.length_squared() == 0) {
					forward = cameraTransform->Right().cross(iw::vector3::unit_y);
				}

				delta += forward * movement.z * speed * Time::DeltaTime();
			}

			cameraTransform->Position += delta;
		}
			
		if (rotation != 0) {
			iw::quaternion deltaP = iw::quaternion::from_axis_angle(cameraTransform->Right(), rotation.x);
			iw::quaternion deltaY = iw::quaternion::from_axis_angle(-iw::vector3::unit_y, rotation.y);

			cameraTransform->Rotation = deltaP * cameraTransform->Rotation;
			cameraTransform->Rotation = deltaY * cameraTransform->Rotation;
		}

		rotation = 0; // reset rotation
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
		if (e.Device == DeviceType::RAW_MOUSE) {
			switch (e.Button) {
				case RMOUSE: speed  = e.State ? 50.0f : 5.0f; break;
				case MMOUSE: active = e.State;                break;
			}
		}

		return false;
	}

	bool EditorCameraControllerSystem::On(
		MouseWheelEvent& e)
	{
		if (e.Device == DeviceType::MOUSE) {
			cameraTransform->Position += e.Delta * cameraTransform->Forward() * speed;
		}

		return false;
	}
}
