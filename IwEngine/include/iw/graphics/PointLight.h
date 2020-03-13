#pragma once

#include "Light.h"
#include "Camera.h"

namespace iw {
namespace Graphics {
	class PointLight
		: public Light
	{
	protected:
		float m_radius;

	public:
		// Takes ownership of camera ptr
		IWGRAPHICS_API
		PointLight(
			float             radius       = 30.0f,
			float             intensity    = 10.0f,
			ref<Shader>       shadowShader = nullptr,
			ref<RenderTarget> shadowTarget = nullptr);

		IWGRAPHICS_API
		~PointLight() override = default;

		IWGRAPHICS_API
		virtual void SetupShadowCast(
			Renderer* renderer);

		IWGRAPHICS_API float  Radius() const;
		IWGRAPHICS_API float& Radius();

		IWGRAPHICS_API
		void SetRadius(
			float radius);

		void SetShadowTarget(
			ref<RenderTarget>& shadowTarget) override;
	};
}

	using namespace Graphics;
}
