#include "iw/graphics/Light.h"

namespace iw {
namespace Graphics {
	Light::Light(
		float power,
		iw::Camera* camera,
		iw::ref<Shader> shadowShader,
		iw::ref<RenderTarget> shadowTarget)
		: m_intensity(power)
		, m_shadowCamera(camera)
		, m_shadowShader(shadowShader)
		, m_shadowTarget(shadowTarget)
	{
#ifdef IW_DEBUG
		if (   (!shadowShader && !shadowTarget)
			|| ( shadowShader &&  shadowTarget))
		{
			LOG_WARNING << "A light needs both a shader and a target or neither.";
		}
#endif
	}

	Light::~Light() {
		delete m_shadowCamera;
	}

	bool Light::CanCastShadows() const {
		return m_shadowCamera && m_shadowTarget;
	}

	float Light::Intensity() const {
		return m_intensity;
	}

	float& Light::Intensity() {
		return m_intensity;
	}

	const vector3& Light::Position() const {
		return m_shadowCamera->Position();
	}

	vector3& Light::Position() {
		return m_shadowCamera->Position();
	}

	const iw::ref<Shader>& Light::ShadowShader() const {
		return m_shadowShader;
	}

	iw::ref<Shader>& Light::ShadowShader() {
		return m_shadowShader;
	}

	const iw::ref<RenderTarget>& Light::ShadowTarget() const {
		return m_shadowTarget;
	}

	iw::ref<RenderTarget>& Light::ShadowTarget() {
		return m_shadowTarget;
	}

	void Light::SetIntensity(
		float intensity)
	{
		m_intensity = intensity;
	}

	void Light::SetPosition(
		const vector3& position)
	{
		m_shadowCamera->SetPosition(position);
	}

	void Light::SetShadowShader(
		ref<Shader>& shadowShader)
	{
		m_shadowShader = shadowShader;
	}

	void Light::SetShadowTarget(
		ref<RenderTarget>& shadowTarget)
	{
		m_shadowTarget = shadowTarget;
	}

	/*void Light::PostProcess() {}

	const iw::ref<Shader>& Light::LightShader() const {
		return m_light;
	}

	const iw::ref<Shader>& Light::NullFilter() const {
		return m_null;
	}

	const iw::ref<RenderTarget>& Light::ShadowTarget() const {
		return m_shadowTarget;
	}

	const iw::ref<Shader>& Light::PostFilter() const {
		return m_post;
	}

	const iw::ref<RenderTarget>& Light::PostTarget() const {
		return m_postTarget;
	}

	void Light::SetLightShader(
		iw::ref<Shader>& lightShader)
	{
		m_light = lightShader;
	}

	void Light::SetNullFilter(
		iw::ref<Shader>& nullFilter)
	{
		m_null = nullFilter;
	}

	void Light::SetShadowTarget(
		iw::ref<RenderTarget>& shadowTarget)
	{
		m_shadowTarget = shadowTarget;
	}

	void Light::SetPostFilter(
		iw::ref<Shader>& post)
	{
		m_post = post;
	}

	void Light::SetPostTarget(
		iw::ref<RenderTarget>& postTarget)
	{
		m_postTarget = postTarget;
	}*/
}
}
