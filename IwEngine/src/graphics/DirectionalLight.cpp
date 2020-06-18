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
		: Light(intensity, shadowTarget, shadowShader, particleShadowShader)
		, m_shadowCamera(shadowCamera)
	{}

	void DirectionalLight::SetupShadowCast(
		Renderer* renderer)
	{
		renderer->SetCamera(&m_shadowCamera);
		//renderer->Device->SetCullFace(BACK);
	}

	matrix4 DirectionalLight::ViewProjection() const {
		return m_shadowCamera.ViewProjection();
	}

	const quaternion& DirectionalLight::Rotation() const {
		return m_shadowCamera.Rotation();
	}

	void DirectionalLight::SetRotation(
		const iw::quaternion& rotation)
	{
		m_shadowCamera.SetRotation(rotation);
	}

	Camera* DirectionalLight::ShadowCamera() const {
		return (Camera*)&m_shadowCamera;
	}
}
}
