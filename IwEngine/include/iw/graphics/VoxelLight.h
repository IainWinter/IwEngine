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
		vector3 m_position;

	public:
		// DOES NOT TAKE OWNERSHIP OF MAIN CAMERA
		IWGRAPHICS_API
		VoxelLight(
			ref<Texture> voxelTexture,
			ref<Shader>  voxelizerShader,
			ref<Shader>  particleVoxelizerShader = nullptr);

		GEN_copy(IWGRAPHICS_API, VoxelLight)
		GEN_move(IWGRAPHICS_API, VoxelLight)

		const vector3& Position() const override;
		void SetPosition(const vector3& position) override;

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
