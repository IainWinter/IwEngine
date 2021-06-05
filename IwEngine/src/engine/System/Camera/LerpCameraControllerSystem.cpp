#include "iw/engine/Systems/Camera/LerpCameraControllerSystem.h"

namespace iw {
	LerpCameraControllerSystem::LerpCameraControllerSystem(
		Entity target,
		Camera* camera)
		: SystemBase("Lerp Camera Controller")
		, Target(target)
		, Locked(false)
		, Follow(true)
		, Speed(20.0)
		, camera(camera)
	{}

	void LerpCameraControllerSystem::Update() {
		if (Locked || !camera) {
			return;
		}

		glm::vec3 target;
		if (Target != EntityHandle::Empty && Follow) {
			target = Target.Find<Transform>()->Position;
		}

		glm::quat camrot = glm::angleAxis(iw::Pi, glm::vec3(0, 1, 0));

		camera->SetPosition(iw :: lerp(camera->Position(), target, Time::DeltaTime() * Speed));
		camera->SetRotation(glm::slerp(camera->Rotation(), camrot, Time::DeltaTime() * Speed));
	}
}
