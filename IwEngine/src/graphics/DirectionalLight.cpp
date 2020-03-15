#include "iw/graphics/DirectionalLight.h"
#include "iw/graphics/Renderer.h"

namespace iw {
namespace Graphics {
	DirectionalLight::DirectionalLight(
		float intensity,
		OrthographicCamera shadowCamera,
		ref<Shader> shadowShader,
		ref<RenderTarget> shadowTarget)
		: Light(intensity, nullptr, shadowShader, shadowTarget)
	{
		m_shadowCamera = new OrthographicCamera(shadowCamera);
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
