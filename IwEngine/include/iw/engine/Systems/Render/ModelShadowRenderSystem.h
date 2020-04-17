#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/graphics/Model.h"

namespace iw {
namespace Engine {
	class ModelShadowRenderSystem
		: public System<Transform, Model>
	{
	public:
		struct Components {
			Transform* Transform;
			Model* Model;
		};

		Scene* m_scene;

	public:
		IWENGINE_API
		ModelShadowRenderSystem(
			Scene* scene);

		IWENGINE_API
		void Update(
			EntityComponentArray& view);
	};
}

	using namespace Engine;
}
