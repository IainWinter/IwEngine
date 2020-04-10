#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"

namespace iw {
namespace Engine {
	class DrawCollidersSystem
		: public System<>
	{
	private:
		Camera* m_camera;

		ref<Material> material;
		Mesh* capsule;
		Mesh* sphere;
		Mesh* plane;

	public:
		IWENGINE_API
		DrawCollidersSystem(
			Camera* camera);

		IWENGINE_API
		int Initialize() override;

		IWENGINE_API
		void Update() override;
	};
}

	using namespace Engine;
}
