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
		cube[0] = m_shadowCamera->ViewProjection();                                       // forward
		cube[1] = m_shadowCamera->ViewProjection() * matrix4::create_rotation_y( Pi / 2); // left
		cube[2] = m_shadowCamera->ViewProjection() * matrix4::create_rotation_y( Pi);     // back
		cube[3] = m_shadowCamera->ViewProjection() * matrix4::create_rotation_y(-Pi);     // right
		cube[4] = m_shadowCamera->ViewProjection() * matrix4::create_rotation_x( Pi);     // top
		cube[5] = m_shadowCamera->ViewProjection() * matrix4::create_rotation_x(-Pi);     // bottom

		m_shadowShader->Handle()->GetParam("light_cubemats")->SetAsFloats(cube, 16, 16 * 6);
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
}
}
