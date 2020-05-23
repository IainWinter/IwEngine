#pragma once

#include "Light.h"

namespace iw {
namespace Graphics {
	struct PointLight
		: Light
	{
	private:
		float m_radius;
		bool m_outdated;
		PerspectiveCamera m_shadowCamera;

	public:
		IWGRAPHICS_API
		PointLight(
			float             radius       = 30.0f,
			float             intensity    = 10.0f,
			ref<RenderTarget> shadowTarget = nullptr,
			ref<Shader>       shadowShader = nullptr,
			ref<Shader>       particleShadowShader = nullptr);

		IWGRAPHICS_API
		void SetupShadowCast(
			Renderer* renderer) override;

		IWGRAPHICS_API
		bool Outdated() const override;

		IWGRAPHICS_API
		float Radius() const;

		IWGRAPHICS_API
		void SetRadius(
			float radius);

		IWGRAPHICS_API
		void SetShadowTarget(
			ref<RenderTarget>& shadowTarget) override;

		IWGRAPHICS_API
		Camera* ShadowCamera() const override;
	private:
		void UpdateCamera();
	};
}

	using namespace Graphics;
}
