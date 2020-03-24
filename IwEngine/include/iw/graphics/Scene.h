#pragma once

#include "Camera.h"
#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include <vector>

namespace iw {
namespace Graphics {
	struct Scene {
	private:
		Camera* m_camera;
		float m_ambiance;
		std::vector<PointLight*> m_pointLights;
		std::vector<DirectionalLight*> m_directionalLights;
		std::vector<Light*> m_lights;

	public:
		IWGRAPHICS_API
		Scene();

		// references camera
		IWGRAPHICS_API
		void SetMainCamera(
			Camera* camera);

		IWGRAPHICS_API
		void SetAmbiance(
			float ambiance);

		IWGRAPHICS_API
		void AddLight(
			PointLight* pointLight);

		IWGRAPHICS_API
		void AddLight(
			DirectionalLight* pointLight);

		IWGRAPHICS_API
		void RemoveLight(
			const Light* light);

		IWGRAPHICS_API
		Camera* MainCamera();

		IWGRAPHICS_API
		float&  Ambiance();

		IWGRAPHICS_API

		std::vector<Light*>            Lights();
		IWGRAPHICS_API

		std::vector<PointLight*>       PointLights();
		IWGRAPHICS_API
		std::vector<DirectionalLight*> DirectionalLights();
	};
}

	using namespace Graphics;
}
