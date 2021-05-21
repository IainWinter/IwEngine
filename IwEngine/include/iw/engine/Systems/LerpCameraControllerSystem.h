#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/graphics/Scene.h"

namespace iw {
namespace Engine {
	class LerpCameraControllerSystem
		: public System<Transform, Camera*>
	{
	public:
		struct Components {
			Transform* Transform;
			Camera**    Camera;
		};
	private:
		bool locked;
		bool follow;

		float speed;

		Entity m_target;

	public:
		IWENGINE_API
		LerpCameraControllerSystem(
			Entity target);

		void SetLocked(
			bool locked)
		{
			locked = true;
		}

		IWENGINE_API
		void Update(
			EntityComponentArray& eca) override;
	};
}
}
