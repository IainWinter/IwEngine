#pragma once

#include "Light.h"
#include "ComputeShader.h"

namespace iw {
namespace Graphics {
	struct VoxelLight
		: public Light
	{
	private:
		ref<Texture> m_voxelTexture;
		ref<ComputeShader> m_mipmapGenerationShader;
		OrthographicCamera m_shadowCamera;

	public:
		IWGRAPHICS_API
		VoxelLight(
			ref<Texture>       voxelTexture,
			ref<Shader>        voxelizerShader,
			ref<Shader>        particleVoxelizerShader = nullptr,
			ref<ComputeShader> mipmapGenerationShader  = nullptr);

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

		IWGRAPHICS_API
		Camera* ShadowCamera() const override;
	};
}

	using namespace Graphics;
}
