#pragma once

#include "Light.h"

namespace IW {
namespace Graphics {
	struct DirectionalLight
		: Light
	{
	private:
		OrthographicCamera m_camera;

	public:
		IWGRAPHICS_API
		DirectionalLight() = default;

		IWGRAPHICS_API
		DirectionalLight(
			iw::ref<Shader> shader,
			iw::ref<IW::RenderTarget> target,
			OrthographicCamera camera);

		IWGRAPHICS_API
		void SetPosition(
			const iw::vector3& position) override;

		IWGRAPHICS_API
		void SetRotation(
			const iw::quaternion& rotation) override;

		IWGRAPHICS_API
		Camera& Cam() override;
	};
}

	using namespace Graphics;
}
