#include "iw/graphics/Light.h"

namespace iw {
namespace Graphics {
	Light::Light(
		iw::ref<Shader> shader,
		iw::ref<iw::RenderTarget> target)
		: m_lightShader(shader)
		, m_lightTarget(target)
	{}

	const iw::ref<Shader>& Light::LightShader() const {
		return m_lightShader;
	}

	const iw::ref<RenderTarget>& Light::LightTarget() const {
		return m_lightTarget;
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
