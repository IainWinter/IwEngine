#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/graphics/Model.h"

namespace iw {
namespace Engine {
	class LightRenderSystem
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
		LightRenderSystem(
			Scene* scene);

		IWENGINE_API
		void Update(
			EntityComponentArray& view);
	};
}

	using namespace Engine;
}
