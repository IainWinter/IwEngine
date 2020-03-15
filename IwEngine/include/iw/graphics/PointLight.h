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
		bool m_outdated;

	public:
		// Takes ownership of camera ptr
		IWGRAPHICS_API
		PointLight(
			float             radius       = 30.0f,
			float             intensity    = 10.0f,
			ref<Shader>       shadowShader = nullptr,
			ref<RenderTarget> shadowTarget = nullptr);

		GEN_copy(IWGRAPHICS_API, PointLight)
		GEN_move(IWGRAPHICS_API, PointLight)

		IWGRAPHICS_API
		~PointLight() override = default;

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
	private:
		void UpdateCamera();
	};
}

	using namespace Graphics;
}
