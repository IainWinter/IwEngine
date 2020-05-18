#pragma once

#include "Light.h"
#include "Camera.h"

namespace iw {
namespace Graphics {

	// monksas

	class VoxelLight
		: public Light
	{
	private:
		ref<Texture> m_voxelTexture;
	public:
		// DOES NOT TAKE OWNERSHIP OF MAIN CAMERA
		IWGRAPHICS_API
		VoxelLight(
			Camera*      mainCamera,
			ref<Texture> voxelTexture,
			ref<Shader>  voxelizerShader,
			ref<Shader>  particleVoxelizerShader = nullptr);

		GEN_copy(IWGRAPHICS_API, VoxelLight)
		GEN_move(IWGRAPHICS_API, VoxelLight)

		IWGRAPHICS_API
		~VoxelLight() override;

		IWGRAPHICS_API
		void SetupShadowCast(
			Renderer* renderer) override;

		IWGRAPHICS_API
		void EndShadowCast(
			Renderer* renderer) override;

		IWGRAPHICS_API
		bool CanCastShadows() const override;

		IWGRAPHICS_API
		ref<Texture> VoxelTexture();
	};
}

	using namespace Graphics;
}
