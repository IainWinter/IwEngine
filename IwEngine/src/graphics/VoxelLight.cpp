#include "iw/graphics/VoxelLight.h"
#include "iw/graphics/QueuedRenderer.h"

#include "gl/glew.h"

namespace iw {
namespace Graphics {
	VoxelLight::VoxelLight(
		ref<Texture> voxelTexture,
		ref<Shader>  voxelizerShader,
		ref<Shader>  particleVoxelizerShader)
		: Light(0, nullptr, nullptr, voxelizerShader, particleVoxelizerShader)
		, m_voxelTexture(voxelTexture)
	{ }

	VoxelLight::VoxelLight(
		const VoxelLight& copy)
		: Light(0, nullptr, nullptr, copy.m_shadowShader, copy.m_particleShadowShader)
		, m_voxelTexture(copy.m_voxelTexture)
	{}

	VoxelLight::VoxelLight(
		VoxelLight&& copy) noexcept
		: Light(0, nullptr, nullptr, copy.m_shadowShader, copy.m_particleShadowShader)
		, m_voxelTexture(copy.m_voxelTexture)
	{
		copy.m_shadowShader         = nullptr;
		copy.m_particleShadowShader = nullptr;
		copy.m_voxelTexture         = nullptr;
	}

	VoxelLight& VoxelLight::operator=(
		const VoxelLight& copy)
	{
		m_shadowShader         = copy.m_shadowShader;
		m_particleShadowShader = copy.m_particleShadowShader;
		m_voxelTexture         = copy.m_voxelTexture;
		
		return *this;
	}

	VoxelLight& VoxelLight::operator=(
		VoxelLight&& copy) noexcept
	{
		m_shadowShader         = copy.m_shadowShader;
		m_particleShadowShader = copy.m_particleShadowShader;
		m_voxelTexture         = copy.m_voxelTexture;

		copy.m_shadowShader         = nullptr;
		copy.m_particleShadowShader = nullptr;
		copy.m_voxelTexture         = nullptr;

		return *this;
	}

	void VoxelLight::SetupShadowCast(
		Renderer* renderer)
	{
		renderer->SetTarget(nullptr);
		renderer->Device->SetViewport(m_shadowTarget->Tex(0)->Width(), m_shadowTarget->Tex(0)->Height());

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDisable(GL_DEPTH_TEST);

		// not sure if these are needed
		//glDisable(GL_CULL_FACE);
		//glDisable(GL_BLEND);

		IPipelineParam* out = m_shadowShader->Handle()->GetParam("voxelTexture");
		if (out) {
			out->SetAsTexture(m_shadowTarget->Tex(0)->Handle());
		}
	}

	void VoxelLight::EndShadowCast(
		Renderer* renderer)
	{
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_CULL_FACE);
		//glEnable(GL_BLEND);

		Light::EndShadowCast(renderer);
	}
}
}
