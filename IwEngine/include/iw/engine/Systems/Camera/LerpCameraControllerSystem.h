#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/graphics/Scene.h"
#include "iw/engine/Systems/Camera/CameraController.h"

namespace iw {
namespace Engine {
	class LerpCameraControllerSystem
		: public SystemBase
		, public CameraController
	{
	public:
		Entity& Target;

		glm::vec3 Offset;

		bool Locked;
		bool Follow;
		float Speed;
	private:
		iw::Camera* camera;

	public:
		IWENGINE_API
		LerpCameraControllerSystem(
			Entity& target,
			Camera* camera = nullptr,
			glm::vec3 offset = glm::vec3(0));

		IWENGINE_API
		void Update() override;

		Camera* GetCamera() override {
			return camera;
		}
	};
}
}
