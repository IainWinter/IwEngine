#include "iw/engine/Systems/LerpCameraControllerSystem.h"
#include "iw/engine/Time.h"
#include <imgui/imgui.h>

#include "glm/gtx/compatibility.hpp"

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

			glm::vec3 target;
			if (m_target != iw::EntityHandle::Empty && follow) {
				target = m_target.Find<iw::Transform>()->Position;
			}

			target.y += 2;
			target.x -= 5;

			glm::quat camrot = glm::angleAxis(glm::pi<float>() * 0.5f, glm::vec3(0, 1, 0));

			t->Position = glm::lerp(t->Position, target, iw::Time::DeltaTime() * speed);
			t->Rotation = glm::lerp(t->Rotation, camrot, iw::Time::DeltaTime() * speed);
		}
	}
}
