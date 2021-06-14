#include "iw/engine/Systems/Camera/LerpCameraControllerSystem.h"

namespace iw {
	LerpCameraControllerSystem::LerpCameraControllerSystem(
		Entity& target,
		Camera* camera,
		glm::vec3 offset)
		: SystemBase("Lerp Camera Controller")
		, Target(target)
		, camera(camera)
		, Offset(offset)
		, Locked(false)
		, Follow(true)
		, Speed(20.0)
	{}

	void LerpCameraControllerSystem::Update() {
		if (Locked || !camera) {
			return;
		}

		glm::vec3 target = Offset;
		if (Target != EntityHandle::Empty && Follow) {
			target += Target.Find<Transform>()->Position;
		}

		glm::quat camrot = glm::angleAxis(iw::Pi, glm::vec3(0, 1, 0));

		float delta = clamp(Time::DeltaTime() * Speed, 0.f, 1.f);

		camera->SetPosition(iw :: lerp(camera->Position(), target, delta));
		camera->SetRotation(glm::slerp(camera->Rotation(), camrot, delta));
	}
}
