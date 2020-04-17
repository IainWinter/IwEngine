#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/graphics/Mesh.h"
#include "iw/graphics/Scene.h"

namespace iw {
namespace Engine {
	class MeshRenderSystem
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
		MeshRenderSystem(
			Scene* m_scene);

		IWENGINE_API
		void Update(
			EntityComponentArray& view);
	};
}

	using namespace Engine;
}
