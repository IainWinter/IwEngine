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
		Entity Target;

		bool Locked;
		bool Follow;
		float Speed;
	private:
		iw::Camera* camera;

	public:
		IWENGINE_API
		LerpCameraControllerSystem(
			Entity target,
			Camera* camera = nullptr);

		IWENGINE_API
		void Update() override;

		Camera* GetCamera() override {
			return camera;
		}
	};
}
}
