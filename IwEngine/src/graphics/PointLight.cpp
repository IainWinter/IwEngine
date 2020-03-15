#include "iw/graphics/PointLight.h"
#include "iw/graphics/Renderer.h"

namespace iw {
namespace Graphics {
	PointLight::PointLight(
		float radius,
		float power,
		ref<Shader> shadowShader,
		ref<RenderTarget> shadowTarget)
		: Light(power, nullptr, shadowShader, shadowTarget)
		, m_radius(radius)
		, m_outdated(true)
	{
		UpdateCamera();
	}

	PointLight::PointLight(
		const PointLight& copy)
		: Light(m_intensity, nullptr, copy.m_shadowShader, copy.m_shadowTarget)
		, m_radius(copy.m_radius)
		, m_outdated(true)
	{
		UpdateCamera();
	}

	PointLight::PointLight(
		PointLight&& copy) noexcept
		: Light(m_intensity, copy.m_shadowCamera, copy.m_shadowShader, copy.m_shadowTarget)
		, m_radius(copy.m_radius)
		, m_outdated(true)
	{
		copy.m_shadowCamera = nullptr;
		copy.m_shadowShader = nullptr;
		copy.m_shadowTarget = nullptr;
	}

	PointLight& PointLight::operator=(
		const PointLight& copy)
	{
		m_intensity    = copy.m_intensity;
		m_shadowCamera = nullptr;
		m_shadowShader = copy.m_shadowShader;
		m_shadowTarget = copy.m_shadowTarget;
		m_radius       = copy.m_radius;
		m_outdated     = true;

		UpdateCamera();

		return *this;
	}

	PointLight& PointLight::operator=(
		PointLight&& copy) noexcept
	{
		m_intensity    = copy.m_intensity;
		m_shadowCamera = copy.m_shadowCamera;
		m_shadowShader = copy.m_shadowShader;
		m_shadowTarget = copy.m_shadowTarget;
		m_radius       = copy.m_radius;
		m_outdated     = true;

		copy.m_shadowCamera = nullptr;
		copy.m_shadowShader = nullptr;
		copy.m_shadowTarget = nullptr;

		return *this;
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
		cube[0] = matrix4::create_look_at(Position(), Position() + vector3::unit_x, -vector3::unit_y) * m_shadowCamera->Projection();
		cube[1] = matrix4::create_look_at(Position(), Position() - vector3::unit_x, -vector3::unit_y) * m_shadowCamera->Projection();
		cube[2] = matrix4::create_look_at(Position(), Position() + vector3::unit_y,  vector3::unit_z) * m_shadowCamera->Projection();
		cube[3] = matrix4::create_look_at(Position(), Position() - vector3::unit_y, -vector3::unit_z) * m_shadowCamera->Projection();
		cube[4] = m_shadowCamera->ViewProjection();
		cube[5] = matrix4::create_look_at(Position(), Position() - vector3::unit_z, -vector3::unit_y) * m_shadowCamera->Projection();

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

	void PointLight::UpdateCamera() {
		if (CanCastShadows()) {
#ifdef IW_DEBUG
			if (m_shadowTarget->Width() != m_shadowTarget->Height()) {
				LOG_WARNING << "Shadow maps need to have textures with equal width and height";
			}
#endif 
			PerspectiveCamera* camera = new PerspectiveCamera(
				Position(), quaternion::identity,
				Pi / 2,
				m_shadowTarget->Width() / m_shadowTarget->Height(),
				0.01f,
				Radius()
			);
			
			camera->SetRotation(quaternion::from_axis_angle(vector3::unit_z, Pi));

			delete m_shadowCamera;
			m_shadowCamera = camera;
		}

		else {
			m_shadowCamera = new PerspectiveCamera();
		}

		m_outdated = true;
	}
}
}
