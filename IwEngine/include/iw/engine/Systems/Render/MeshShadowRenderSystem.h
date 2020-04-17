#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/graphics/Model.h"

namespace iw {
namespace Engine {
	class MeshShadowRenderSystem
		: public System<Transform, Mesh>
	{
	public:
		struct Components {
			Transform* Transform;
			Mesh* Mesh;
		};

		Scene* m_scene;

	public:
		IWENGINE_API
		MeshShadowRenderSystem(
			Scene* scene);

		IWENGINE_API
		void Update(
			EntityComponentArray& view);
	};
}

	using namespace Engine;
}
