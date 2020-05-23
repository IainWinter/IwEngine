#include "iw/graphics/VoxelLight.h"
#include "iw/graphics/QueuedRenderer.h"
#include "iw/engine/Time.h"

#include "gl/glew.h"

namespace iw {
namespace Graphics {
	VoxelLight::VoxelLight(
		ref<Texture> voxelTexture,
		ref<Shader>  voxelizerShader,
		ref<Shader>  particleVoxelizerShader)
		: Light(0, nullptr, voxelizerShader, particleVoxelizerShader)
		, m_voxelTexture(voxelTexture)
		, m_shadowCamera(2, 2, -1, 1)
	{}

	void VoxelLight::SetupShadowCast(
		Renderer* renderer)
	{
		renderer->Device->SetViewport(m_voxelTexture->Width(), m_voxelTexture->Height());
		renderer->SetCamera(ShadowCamera());

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		if (!m_voxelTexture->Handle()) {
			m_voxelTexture->Initialize(renderer->Device);
			m_voxelTexture->SetBorderColor(Color(0, 0, 0, 0));
		}

		m_voxelTexture->Clear();

		IPipelineParam* textureParam = m_shadowShader->Handle()->GetParam("voxelTexture");
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

		m_voxelTexture->Handle()->GenerateMipMaps();
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
}
}
