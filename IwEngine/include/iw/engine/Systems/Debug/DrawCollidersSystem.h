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

		MeshDescription description;

		Mesh sphereInstance;
		Mesh planeInstance;
		Mesh cubeInstance;

		ref<Material> material;

		std::unordered_map<std::pair<float, float>, Mesh, pair_hash> capsules;

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
