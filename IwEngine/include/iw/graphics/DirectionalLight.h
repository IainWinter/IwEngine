#pragma once

#include "Light.h"

namespace iw {
namespace Graphics {
	struct DirectionalLight
		: Light
	{
	private:
		OrthographicCamera m_shadowCamera;
	public:
		IWGRAPHICS_API
		DirectionalLight(
			float              intensity    = 10.0f,
			OrthographicCamera shadowCamera = OrthographicCamera(32, 32, -100, 100),
			ref<RenderTarget>  shadowTarget = nullptr,
			ref<Shader>        shadowShader = nullptr,
			ref<Shader>        particleShadowShader = nullptr);

		IWGRAPHICS_API
		void SetupShadowCast(
			Renderer* renderer) override;

		IWGRAPHICS_API
		matrix4 ViewProjection() const;

		IWGRAPHICS_API quaternion Rotation() const;
		IWGRAPHICS_API quaternion WorldRotation() const;

		IWGRAPHICS_API
		void SetRotation(
			const quaternion& rotation);

		IWGRAPHICS_API
		Camera* ShadowCamera() const override;
	};
}

	using namespace Graphics;
}
