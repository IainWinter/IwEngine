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
	{
		if (CanCastShadows()) {
			m_shadowCamera = new PerspectiveCamera(
				Pi / 2,
				shadowTarget->Width() / shadowTarget->Height(),
				0.01f,
				radius
			);
		}

		else {
			m_shadowCamera = new PerspectiveCamera();
		}
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
		cube[0] = matrix4::create_look_at(Position(), Position() + vector3( 1.0,  0.0,  0.0), vector3(0.0, 1.0,  0.0)) * m_shadowCamera->Projection(); 
		cube[1] = matrix4::create_look_at(Position(), Position() + vector3(-1.0,  0.0,  0.0), vector3(0.0, 1.0,  0.0)) * m_shadowCamera->Projection();
		cube[2] = matrix4::create_look_at(Position(), Position() + vector3( 0.0,  1.0,  0.0), vector3(0.0,  0.0, -1.0)) * m_shadowCamera->Projection();
		cube[3] = matrix4::create_look_at(Position(), Position() + vector3( 0.0, -1.0,  0.0), vector3(0.0,  0.0, 1.0)) * m_shadowCamera->Projection(); 
		cube[4] = matrix4::create_look_at(Position(), Position() + vector3( 0.0,  0.0,  1.0), vector3(0.0, 1.0,  0.0)) * m_shadowCamera->Projection();
		cube[5] = matrix4::create_look_at(Position(), Position() + vector3( 0.0,  0.0, -1.0), vector3(0.0, 1.0,  0.0)) * m_shadowCamera->Projection(); 

		m_shadowShader->Handle()->GetParam("light_mats[0]") ->SetAsMat4s(cube, 6);
		m_shadowShader->Handle()->GetParam("light_pos")     ->SetAsFloats(&Position(), 3);
		m_shadowShader->Handle()->GetParam("light_farPlane")->SetAsFloat(Radius());
	}

	float PointLight::Radius() const {
		return m_radius;
	}

	float& PointLight::Radius() {
		return m_radius;
	}

	void PointLight::SetRadius(
		float radius)
	{
		m_radius = radius;
	}

	void PointLight::SetShadowTarget(
		ref<RenderTarget>& shadowTarget)
	{
		PerspectiveCamera* camera = new PerspectiveCamera(
			Position(), quaternion::identity,
			Pi / 2,
			shadowTarget->Width() / shadowTarget->Height(),
			0.01f,
			Radius()
		);

		delete m_shadowCamera;
		m_shadowCamera = camera;

		Light::SetShadowTarget(shadowTarget);
	}
}
}
