#pragma once 

#include "iw/engine/Layer.h"
#include "iw/graphics/Font.h"
#include "iw/graphics/PointLight.h"

namespace iw {
	// Test for intel graphics
	class TestLayer
		: public Layer
	{
	private:
		Scene* scene;
		ref<Shader> shader;

		Camera* camera;
		PointLight* light;
		PointLight* light2;
		DirectionalLight* dirLight;

	public:
		TestLayer();

		int Initialize() override;

		void PostUpdate() override;
		void FixedUpdate() override;

		void ImGui() override;
	};
}
