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
		ref<RenderTarget> m_target;
		bool m_clear;
		Color m_clearColor;

		ComponentQuery m_models;
		ComponentQuery m_meshes;

	public:
		IWENGINE_API
		RenderSystem(
			Scene* scene,
			ref<RenderTarget> target = nullptr,
			bool clear = false,
			Color clearColor = iw::Color(0));

		IWENGINE_API
		int Initialize() override;

		IWENGINE_API
		void Update()override;
	};
}

	using namespace Engine;
}
