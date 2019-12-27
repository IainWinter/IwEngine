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
		DirectionalLight(
			OrthographicCamera camera);

		IWGRAPHICS_API
		const Camera& Cam() const override;

		IWGRAPHICS_API
		Camera& Cam() override;

		IWGRAPHICS_API
		void SetCam(
			const OrthographicCamera& camera);

		IWGRAPHICS_API
		void SetPosition(
			const iw::vector3& position) override;

		IWGRAPHICS_API
		void SetRotation(
			const iw::quaternion& rotation) override;
	};
}

	using namespace Graphics;
}
