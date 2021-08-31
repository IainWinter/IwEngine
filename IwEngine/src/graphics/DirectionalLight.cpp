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

	glm::mat4 DirectionalLight::ViewProjection() const {
		return m_shadowCamera.ViewProjection();
	}

	glm::quat DirectionalLight::Rotation() const {
		return m_shadowCamera.Transform.Rotation;
	}

	glm::quat DirectionalLight::WorldRotation() const {
		return ShadowCamera()->Transform.WorldRotation();
	}

	void DirectionalLight::SetRotation(
		const glm::quat& rotation)
	{
		m_shadowCamera.Transform.Rotation = rotation;
	}

	Camera* DirectionalLight::ShadowCamera() const {
		return (Camera*)&m_shadowCamera;
	}
}
}
