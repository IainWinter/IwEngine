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

		bool noMipmaps;

	public:
		IWGRAPHICS_API
		VoxelLight(
			ref<Texture>       voxelTexture,
			ref<Shader>        voxelizerShader,
			ref<Shader>        particleVoxelizerShader = nullptr,
			ref<ComputeShader> mipmapGenerationShader  = nullptr);

		IWGRAPHICS_API
		void SetupShadowCast(
			Renderer* renderer,
			bool useParticleShader,
			bool clear) override;

		IWGRAPHICS_API
		void EndShadowCast(
			Renderer* renderer) override;

		IWGRAPHICS_API
		bool CanCastShadows() const override;

		IWGRAPHICS_API
		ref<Texture> VoxelTexture();

		IWGRAPHICS_API
		Camera* ShadowCamera() const override;

		// sets flag for 1 run
		IWGRAPHICS_API
		void BlockGenerationOfMipmaps();
	};
}

	using namespace Graphics;
}
