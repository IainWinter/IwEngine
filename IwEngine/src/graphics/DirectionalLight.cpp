#include "iw/graphics/DirectionalLight.h"

namespace IW {
	DirectionalLight::DirectionalLight(
		iw::ref<Shader> shader,
		iw::ref<IW::RenderTarget> target,
		OrthographicCamera camera)
		: Light(shader, target)
		, m_camera(camera)
	{}

	void DirectionalLight::SetPosition(
		const iw::vector3& position)
	{
		m_camera.Position = position;
	}

	void DirectionalLight::SetRotation(
		const iw::quaternion& rotation)
	{
		m_camera.Rotation = rotation;
	}

	Camera& DirectionalLight::Cam() {
		return m_camera;
	}
}
