#include "iw/graphics/DirectionalLight.h"
#include "iw/graphics/QueuedRenderer.h"

namespace iw {
namespace Graphics {
	DirectionalLight::DirectionalLight(
		float intensity,
		OrthographicCamera shadowCamera,
		ref<RenderTarget>  shadowTarget,
		ref<Shader>        shadowShader,
		ref<Shader>        particleShadowShader)
		: Light(intensity, new OrthographicCamera(shadowCamera), 
			shadowTarget, 
			shadowShader, 
			particleShadowShader)
	{}

	DirectionalLight::DirectionalLight(
		const DirectionalLight& copy)
		: Light(m_intensity, new OrthographicCamera(*(OrthographicCamera*)copy.m_shadowCamera),
			copy.m_shadowTarget, 
			copy.m_shadowShader, 
			copy.m_particleShadowShader)
	{}

	DirectionalLight::DirectionalLight(
		DirectionalLight&& copy) noexcept
		: Light(m_intensity, copy.m_shadowCamera,
			copy.m_shadowTarget,
			copy.m_shadowShader,
			copy.m_particleShadowShader)
	{
		copy.m_shadowCamera = nullptr;
		copy.m_shadowTarget = nullptr;
		copy.m_shadowShader = nullptr;
		copy.m_particleShadowShader = nullptr;
	}

	DirectionalLight& DirectionalLight::operator=(
		const DirectionalLight& copy)
	{
		m_intensity    = copy.m_intensity;
		m_shadowCamera = new OrthographicCamera(*(OrthographicCamera*)copy.m_shadowCamera);
		m_shadowTarget = copy.m_shadowTarget;
		m_shadowShader = copy.m_shadowShader;
		m_particleShadowShader = copy.m_particleShadowShader;

		return *this;
	}

	DirectionalLight& DirectionalLight::operator=(
		DirectionalLight&& copy) noexcept
	{
		m_intensity    = copy.m_intensity;
		m_shadowCamera = copy.m_shadowCamera;
		m_shadowTarget = copy.m_shadowTarget;
		m_shadowShader = copy.m_shadowShader;
		m_particleShadowShader = copy.m_particleShadowShader;

		copy.m_shadowCamera = nullptr;
		copy.m_shadowTarget = nullptr;
		copy.m_shadowShader = nullptr;
		copy.m_particleShadowShader = nullptr;

		return *this;
	}

	void DirectionalLight::SetupShadowCast(
		Renderer* renderer)
	{
		renderer->SetCamera(m_shadowCamera);
	}

	matrix4 DirectionalLight::ViewProjection() const {
		return m_shadowCamera->ViewProjection();
	}

	const quaternion& DirectionalLight::Rotation() const {
		return m_shadowCamera->Rotation();
	}

	//quaternion& DirectionalLight::Rotation() {
	//	return m_shadowCamera->Rotation();
	//}

	void DirectionalLight::SetRotation(
		const iw::quaternion& rotation)
	{
		m_shadowCamera->SetRotation(rotation);
	}
}
}
