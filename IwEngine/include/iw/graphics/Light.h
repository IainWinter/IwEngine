#pragma once

#include "Shader.h"
#include "Camera.h"

namespace IW {
namespace Graphics {
	struct Light {
	private:
		iw::ref<Shader> m_light;
		iw::ref<Shader> m_null;
		iw::ref<Shader> m_blur;
		IW::Camera* m_camera;

	public:
		IWGRAPHICS_API
		Light(
			iw::ref<Shader> m_null,
			iw::ref<Shader> m_blur,
			iw::ref<Shader> m_light);

		IWGRAPHICS_API
		~Light();

		IWGRAPHICS_API
		void SetCamera(
			Camera& camera);

		IWGRAPHICS_API
		void SetPosition(
			const iw::vector3& position);

		IWGRAPHICS_API
		void SetRotation(
			const iw::quaternion& rotation);
	};
}

	using namespace Graphics;
}
