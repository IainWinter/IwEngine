#include "..\..\include\Systems\EditorCameraController.h"
#include "iw/engine/Time.h"
#include <imgui\imgui.h>

namespace IW {
	EditorCameraController::EditorCameraController()
		: System("Editor Camera Controller")
		, speed(0)
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
		KeyEvent& e)
	{
		switch (e.Button) {
			case IW::W:     movement.z -= e.State == 0 ? -1 : 1; break;
			case IW::S:     movement.z += e.State == 0 ? -1 : 1; break;
			case IW::A:     movement.x -= e.State == 0 ? -1 : 1; break;
			case IW::D:     movement.x += e.State == 0 ? -1 : 1; break;
			case IW::SHIFT: movement.y -= e.State == 0 ? -1 : 1; break;
			case IW::SPACE: movement.y += e.State == 0 ? -1 : 1; break;
		}

		return false;
	}

	bool EditorCameraController::On(
		MouseMovedEvent& e)
	{
		rotation.x = e.DeltaY * 0.0005f;
		rotation.y = e.DeltaX * 0.0005f; // sens?

		return false;
	}

	bool EditorCameraController::On(
		MouseButtonEvent& e)
	{
		if (e.Button = RMOUSE) {
			speed = e.State ? 50 : 10;
		}

		return false;
	}
}
