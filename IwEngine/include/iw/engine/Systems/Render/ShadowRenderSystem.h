#pragma once

#include "iw/engine/System.h"
#include "iw/graphics/Model.h"

namespace iw {
namespace Engine {
	class ShadowRenderSystem
		: public SystemBase
	{
	private:
		Scene* m_scene;
		ref<ComponentQuery> m_models;
		ref<ComponentQuery> m_meshes;

	public:
		IWENGINE_API
		ShadowRenderSystem(
			Scene* scene);

		IWENGINE_API
		int Initialize() override;

		IWENGINE_API
		void Update() override;
	};
}

	using namespace Engine;
}
