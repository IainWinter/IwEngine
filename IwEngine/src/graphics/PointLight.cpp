#include "iw/graphics/PointLight.h"
#include "iw/graphics/QueuedRenderer.h"

namespace iw {
namespace Graphics {
	PointLight::PointLight(
		float radius,
		float intensity,
		ref<RenderTarget>  shadowTarget,
		ref<Shader>        shadowShader,
		ref<Shader>        particleShadowShader)
		: Light(intensity, shadowTarget, shadowShader, particleShadowShader)
		, m_radius(radius)
		, m_outdated(true)
		, m_shadowCamera()
	{
		UpdateCamera();
	}

	void PointLight::SetupShadowCast(
		Renderer* renderer)
	{
		if (!CanCastShadows()) {
#ifdef IW_DEBUG 
			LOG_WARNING << "Tried to render a shadow map for light that has no shader or target!";
#endif
			return;
		}

		matrix4 cube[6];
		cube[0] = matrix4::create_look_at(Position(), Position() + vector3::unit_x, -vector3::unit_y) * m_shadowCamera.Projection();
		cube[1] = matrix4::create_look_at(Position(), Position() - vector3::unit_x, -vector3::unit_y) * m_shadowCamera.Projection();
		cube[2] = matrix4::create_look_at(Position(), Position() + vector3::unit_y,  vector3::unit_z) * m_shadowCamera.Projection();
		cube[3] = matrix4::create_look_at(Position(), Position() - vector3::unit_y, -vector3::unit_z) * m_shadowCamera.Projection();
		cube[4] =                                                                                       m_shadowCamera.ViewProjection();
		cube[5] = matrix4::create_look_at(Position(), Position() - vector3::unit_z, -vector3::unit_y) * m_shadowCamera.Projection();

		m_shadowShader->Handle()->GetParam("light_mats[0]") ->SetAsMat4s(cube, 6);
		m_shadowShader->Handle()->GetParam("light_pos")     ->SetAsFloats(&Position(), 3);
		m_shadowShader->Handle()->GetParam("light_farPlane")->SetAsFloat(Radius());

		m_outdated = false;
	}

	bool PointLight::Outdated() const {
		return m_outdated || Light::Outdated();
	}

	float PointLight::Radius() const {
		return m_radius;
	}

	void PointLight::SetRadius(
		float radius)
	{
		if (m_radius != radius) {
			m_radius = radius;
			if (m_radius < 0.02f) {
				m_radius = 0.02f;
			}

			UpdateCamera();
		}
	}

	void PointLight::SetShadowTarget(
		ref<RenderTarget>& shadowTarget)
	{
		if (m_shadowTarget != shadowTarget) {
			Light::SetShadowTarget(shadowTarget);
			UpdateCamera();
		}
	}

	Camera* PointLight::ShadowCamera() const {
		return (Camera*)&m_shadowCamera;
	}

	void PointLight::UpdateCamera() {
		if (CanCastShadows()) {
//#ifdef IW_DEBUG
//			if (m_shadowTarget->Width() != m_shadowTarget->Height()) {
//				LOG_WARNING << "Shadow maps need to have textures with equal width and height";
//			}
//#endif
			PerspectiveCamera camera(
				Position(), 
				quaternion::from_axis_angle(vector3::unit_z, Pi),
				Pi / 2,
				m_shadowTarget->Width() / m_shadowTarget->Height(),
				0.01f,
				Radius()
			);

			m_shadowCamera = camera;
		}

		else {
			m_shadowCamera = PerspectiveCamera();
		}

		m_outdated = true;
	}
}
}
