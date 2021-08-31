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
		, m_color(.3f)
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

	void Light::SetupShadowCast(
		Renderer* renderer)
	{}

	void Light::EndShadowCast(
		Renderer* renderer)
	{
		if (m_shadowTarget) {
			m_shadowTarget->Tex(0)->Handle()->GenerateMipMaps();
		}

		renderer->Device->SetCullFace(BACK); // ? not sure what this is doing here
	}

	bool Light::CanCastShadows() const {
		return m_shadowShader && m_shadowTarget;
	}

	float Light::Intensity() const {
		return m_intensity;
	}

	glm::vec3 Light::Color() const {
		return m_color;
	}

	glm::vec3 Light::Position() const {
		return ShadowCamera()->Transform.Position;
	}

	glm::vec3 Light::WorldPosition() const {
		return ShadowCamera()->Transform.WorldPosition();
	}

	ref<RenderTarget> Light::ShadowTarget() const {
		return m_shadowTarget;
	}

	ref<Shader> Light::ShadowShader() const {
		return m_shadowShader;
	}

	ref<Shader> Light::ParticleShadowShader() const {
		return m_particleShadowShader;
	}

	void Light::SetIntensity(
		float intensity)
	{
		m_intensity = intensity;
	}

	void Light::SetColor(
		const glm::vec3& color)
	{
		m_color = color;
	}

	void Light::SetPosition(
		const glm::vec3& position)
	{
		ShadowCamera()->Transform.Position = position;
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
