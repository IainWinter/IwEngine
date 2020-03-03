#pragma once 

#include "iw/engine/Layer.h"
#include "iw/graphics/Font.h"

namespace iw {
	// Test for intel graphics
	class TestLayer
		: public Layer
	{
	private:
		Mesh* sphere;
		Camera* camera;
		Transform* transform;

	public:
		TestLayer();

		int Initialize() override;

		void PostUpdate() override;

		void ImGui() override;
	};
}
