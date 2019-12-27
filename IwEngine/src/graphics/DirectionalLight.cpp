#include "iw/graphics/DirectionalLight.h"

namespace IW {
	DirectionalLight::DirectionalLight(
		OrthographicCamera camera)
		: m_camera(camera)
	{}

	const Camera& DirectionalLight::Cam() const {
		return m_camera;
	}

	Camera& Graphics::DirectionalLight::Cam() {
		return m_camera;
	}

	void DirectionalLight::SetCam(
		const OrthographicCamera& camera)
	{
		m_camera = camera;
	}

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
}
