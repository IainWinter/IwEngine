#include "iw/engine/Systems/LerpCameraControllerSystem.h"
#include "iw/engine/Time.h"
#include <imgui/imgui.h>

namespace iw {
	LerpCameraControllerSystem::LerpCameraControllerSystem(
		iw::Entity target)
		: System("Lerp Camera Controller")
		, m_target(target)
		, locked(false)
		, follow(true)
		, speed(20.0)
	{}

	void LerpCameraControllerSystem::Update(
		iw::EntityComponentArray& eca)
	{
		if (locked == true) {
			return;
		}

		for (auto entity : eca) {
			auto [t, c] = entity.Components.Tie<Components>();

			iw::vector3 target;
			if (m_target != iw::EntityHandle::Empty && follow) {
				target = m_target.Find<iw::Transform>()->Position;
			}

			target.y += 2;
			target.x -= 5;

			iw::quaternion camrot = iw::quaternion::from_axis_angle(iw::vector3::unit_y, iw::Pi / 2);

			t->Position = iw::lerp(t->Position, target, iw::Time::DeltaTime() * speed);
			t->Rotation = iw::lerp(t->Rotation, camrot, iw::Time::DeltaTime() * speed);
		}
	}
}
