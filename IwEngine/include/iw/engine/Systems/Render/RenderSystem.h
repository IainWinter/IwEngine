#pragma once

#include "iw/engine/System.h"
#include "iw/graphics/Scene.h"

namespace iw {
namespace Engine {
	class RenderSystem
		: public SystemBase
	{
	private:
		Scene* m_scene;
		ref<ComponentQuery> m_models;
		ref<ComponentQuery> m_meshes;

	public:
		IWENGINE_API
		RenderSystem(
			Scene* m_scene);

		IWENGINE_API
		int Initialize() override;

		IWENGINE_API
		void Update()override;
	};
}

	using namespace Engine;
}
