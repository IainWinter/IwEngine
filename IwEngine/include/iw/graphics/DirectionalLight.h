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
			OrthographicCamera shadowCamera = OrthographicCamera(32, 32, -500, 500),
			ref<RenderTarget>  shadowTarget = nullptr,
			ref<Shader>        shadowShader = nullptr,
			ref<Shader>        particleShadowShader = nullptr);

		IWGRAPHICS_API
		void SetupShadowCast(
			Renderer* renderer) override;

		IWGRAPHICS_API
		glm::mat4 ViewProjection() const;

		IWGRAPHICS_API glm::quat Rotation() const;
		IWGRAPHICS_API glm::quat WorldRotation() const;

		IWGRAPHICS_API
		void SetRotation(
			const glm::quat& rotation);

		IWGRAPHICS_API
		Camera* ShadowCamera() const override;
	};
}

	using namespace Graphics;
}
