#include "iw/graphics/VoxelLight.h"
#include "iw/graphics/QueuedRenderer.h"

#include "gl/glew.h"

namespace iw {
namespace Graphics {
	VoxelLight::VoxelLight(
		ref<Texture> voxelTexture,
		ref<Shader>  voxelizerShader,
		ref<Shader>  particleVoxelizerShader,
		ref<ComputeShader>  mipmapGenerationShader)
		: Light(0, nullptr, voxelizerShader, particleVoxelizerShader)
		, m_voxelTexture(voxelTexture)
		, m_mipmapGenerationShader(mipmapGenerationShader)
		, m_shadowCamera()
	{
		//m_shadowCamera.SetRotation(quaternion::from_euler_angles(0, Pi, 0));
	}

	void VoxelLight::SetupShadowCast(
		Renderer* renderer,
		bool useParticleShader,
		bool clear)
	{
		renderer->Device->SetViewport(m_voxelTexture->Width(), m_voxelTexture->Height());
		//renderer->SetCamera(ShadowCamera());

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		if (!m_voxelTexture->Handle()) {
			m_voxelTexture->Initialize(renderer->Device);
			m_voxelTexture->SetBorderColor(iw::Color(1, 0, 0, 1));
		}

		if (clear) {
			m_voxelTexture->Clear();
		}

		IPipelineParam* textureParam = (useParticleShader ? m_particleShadowShader : m_shadowShader)
										->Handle()->GetParam("voxelTexture");

		if (textureParam) {
			textureParam->SetAsImage(m_voxelTexture->Handle());
		}
	}

	void VoxelLight::EndShadowCast(
		Renderer* renderer)
	{
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);

		if (noMipmaps) {
			noMipmaps = false;
			return;
		}

		if (m_mipmapGenerationShader) {
			renderer->SetShader(m_mipmapGenerationShader);

			IPipelineParam* level  = m_mipmapGenerationShader->Handle()->GetParam("level");
			IPipelineParam* source = m_mipmapGenerationShader->Handle()->GetParam("source");
			IPipelineParam* target = m_mipmapGenerationShader->Handle()->GetParam("target");
		
			source->SetAsTexture(m_voxelTexture->Handle(), 0);

			vector3 dim(m_voxelTexture->Width(), m_voxelTexture->Height(), m_voxelTexture->Depth());
			unsigned levels = log2(dim.major()) + 1;

			int xSize, ySize, zSize;
			m_mipmapGenerationShader->Handle()->GetComputeWorkGroupSize(xSize, ySize, zSize);

			for (int mip = 1; mip < levels; mip++) {
				level ->SetAsInt(mip - 1);
				target->SetAsImage(m_voxelTexture->Handle(), 1, mip);

				int x = ((m_voxelTexture->Width()  >> mip) + xSize - 1) / xSize;
				int y = ((m_voxelTexture->Height() >> mip) + ySize - 1) / ySize;
				int z = ((m_voxelTexture->Depth()  >> mip) + zSize - 1) / zSize;

				renderer->Device->DispatchComputePipeline(m_mipmapGenerationShader->Handle(), x, y, z);
			}
		}

		else {
			m_voxelTexture->Handle()->GenerateMipMaps();
		}
	}

	bool VoxelLight::CanCastShadows() const {
		return true;
	}

	ref<Texture> VoxelLight::VoxelTexture() {
		return m_voxelTexture;
	}
	
	Camera* VoxelLight::ShadowCamera() const {
		return (Camera*)&m_shadowCamera;
	}

	void VoxelLight::BlockGenerationOfMipmaps() {
		noMipmaps = true;
	}
}
}
