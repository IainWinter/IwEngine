#pragma once

#include "Light.h"

namespace iw {
namespace Graphics {
	struct DirectionalLight
		: Light
	{
	public:
		IWGRAPHICS_API
		DirectionalLight(
			float              intensity    = 10.0f,
			OrthographicCamera shadowCamera = OrthographicCamera(32, 32, -100, 100),
			ref<Shader>        shadowShader = nullptr,
			ref<RenderTarget>  shadowTarget = nullptr);

		IWGRAPHICS_API
		~DirectionalLight() override = default;

		IWGRAPHICS_API
		void SetupShadowCast(
			Renderer* renderer) override;

		IWGRAPHICS_API
		matrix4 ViewProjection() const;

		IWGRAPHICS_API const quaternion& Rotation() const;
		//IWGRAPHICS_API       quaternion& Rotation();

		IWGRAPHICS_API
		void SetRotation(
			const quaternion& rotation);
	};
}

	using namespace Graphics;
}
