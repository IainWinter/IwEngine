#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/graphics/Model.h"
#include "iw/graphics/VoxelLight.h"

#include "iw/graphics/ParticleSystem.h"

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

		struct PComponents {
			Transform* Transform;
			ParticleSystem<StaticParticle>* System;
		};

		Scene* m_scene;
		VoxelLight* m_voxelize;

		// For visualization
		ref<RenderTarget> m_front;
		ref<RenderTarget> m_back;

		Mesh m_bounds;
		Mesh m_quad;

		bool m_visualize;

	public:
		IWENGINE_API
		ModelVoxelRenderSystem(
			Scene* m_scene);

		IWENGINE_API
		int Initialize() override;

		IWENGINE_API
		void Update(
			EntityComponentArray& view);

		IWENGINE_API
		ref<Texture> VoxelWorld();
	};
}

	using namespace Engine;
}
