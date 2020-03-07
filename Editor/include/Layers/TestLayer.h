#pragma once 

#include "iw/engine/Layer.h"
#include "iw/graphics/Font.h"

namespace iw {
	// Test for intel graphics
	class TestLayer
		: public Layer
	{
	private:
		Camera* camera;
		iw::vector3 lightPos;
		ref<Shader> shader;

	public:
		TestLayer();

		int Initialize() override;

		void PostUpdate() override;

		void ImGui() override;
	};
}
