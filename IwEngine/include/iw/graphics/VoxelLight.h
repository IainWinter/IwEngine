#pragma once

#include "Light.h"
#include "Camera.h"

namespace iw {
namespace Graphics {

	// monks

	class VoxelLight
		: public Light
	{
	private:
		ref<Texture> m_voxelTexture;
	public:
		// Takes ownership of camera ptr
		IWGRAPHICS_API
		VoxelLight(
			ref<Texture> voxelTexture,
			ref<Shader>  voxelizerShader,
			ref<Shader>  particleVoxelizerShader = nullptr);

		GEN_copy(IWGRAPHICS_API, VoxelLight)
		GEN_move(IWGRAPHICS_API, VoxelLight)

		IWGRAPHICS_API
		~VoxelLight() override = default;

		IWGRAPHICS_API
		void SetupShadowCast(
			Renderer* renderer) override;

		IWGRAPHICS_API
		void EndShadowCast(
			Renderer* renderer) override;

		IWGRAPHICS_API
		bool CanCastShadows() const override;
	};
}

	using namespace Graphics;
}
