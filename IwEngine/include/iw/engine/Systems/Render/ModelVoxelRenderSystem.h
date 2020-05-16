#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/graphics/Model.h"

namespace iw {
namespace Engine {
	class ModelVoxelRenderSystem
		: public System<Transform, Model>
	{
	public:
		struct Components {
			Transform* Transform;
			Model* Model;
		};

		Scene* m_scene;
		Light* m_voxel;

	public:
		IWENGINE_API
		ModelVoxelRenderSystem(
			Scene* m_scene);

		IWENGINE_API
		int Initialize() override;

		IWENGINE_API
		void Update(
			EntityComponentArray& view);
	};
}

	using namespace Engine;
}
