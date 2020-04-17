#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/graphics/Model.h"

namespace iw {
namespace Engine {
	class ModelRenderSystem
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
		ModelRenderSystem(
			Scene* m_scene);

		IWENGINE_API
		void Update(
			EntityComponentArray& view);
	};
}

	using namespace Engine;
}
