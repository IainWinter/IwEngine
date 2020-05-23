#include "iw/graphics/Light.h"
#include "iw/graphics/Renderer.h"

namespace iw {
namespace Graphics {
	Light::Light(
		float intensity,
		iw::ref<RenderTarget> shadowTarget,
		iw::ref<Shader> shadowShader,
		iw::ref<Shader> particleShadowShader)
		: m_intensity(intensity)
		, m_shadowTarget(shadowTarget)
		, m_shadowShader(shadowShader)
		, m_particleShadowShader(particleShadowShader)
	{}

	Light::Light(
		const Light& other)
		: m_intensity(other.m_intensity)
		, m_shadowTarget(other.m_shadowTarget)
		, m_shadowShader(other.m_shadowShader)
		, m_particleShadowShader(other.m_particleShadowShader)
	{}

	Light::Light(
		Light&& other) noexcept
		: m_intensity(other.m_intensity)
		, m_shadowTarget(other.m_shadowTarget)
		, m_shadowShader(other.m_shadowShader)
		, m_particleShadowShader(other.m_particleShadowShader)
	{
		other.m_shadowTarget = nullptr;
		other.m_shadowShader = nullptr;
		other.m_particleShadowShader = nullptr;
	}

	Light& Light::operator=(
		const Light& other)
	{
		m_intensity = other.m_intensity;
		m_shadowTarget = other.m_shadowTarget;
		m_shadowShader = other.m_shadowShader;
		m_particleShadowShader = other.m_particleShadowShader;

		return *this;
	}

	Light& Light::operator=(
		Light&& other) noexcept
	{
		m_intensity = other.m_intensity;
		m_shadowTarget = other.m_shadowTarget;
		m_shadowShader = other.m_shadowShader;
		m_particleShadowShader = other.m_particleShadowShader;

		other.m_shadowTarget = nullptr;
		other.m_shadowShader = nullptr;
		other.m_particleShadowShader = nullptr;

		return *this;
	}

	void Light::EndShadowCast(
		Renderer* renderer)
	{
		if (m_shadowTarget) {
			m_shadowTarget->Tex(0)->Handle()->GenerateMipMaps();
		}

		renderer->Device->SetCullFace(BACK);
	}

	bool Light::CanCastShadows() const {
		return m_shadowShader && m_shadowTarget;
	}

	bool Light::Outdated() const {
		return ShadowCamera()->Outdated();
	}

	float Light::Intensity() const {
		return m_intensity;
	}

	const vector3& Light::Position() const {
		return ShadowCamera()->Position();
	}

	const iw::ref<RenderTarget>& Light::ShadowTarget() const {
		return m_shadowTarget;
	}

	const iw::ref<Shader>& Light::ShadowShader() const {
		return m_shadowShader;
	}

	const ref<Shader>& Light::ParticleShadowShader() const {
		return m_particleShadowShader;
	}

	void Light::SetIntensity(
		float intensity)
	{
		m_intensity = intensity;
	}

	void Light::SetPosition(
		const vector3& position)
	{
		ShadowCamera()->SetPosition(position);
	}

	void Light::SetShadowTarget(
		ref<RenderTarget>& shadowTarget)
	{
		m_shadowTarget = shadowTarget;
	}

	void Light::SetShadowShader(
		ref<Shader>& shadowShader)
	{
		m_shadowShader = shadowShader;
	}

	void Light::SetParticleShadowShader(
		ref<Shader>& particleShadowShader)
	{
		m_particleShadowShader = particleShadowShader;
	}
}
}
