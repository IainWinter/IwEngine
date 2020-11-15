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
		ref<Shader> shader;

		ref<RenderTarget> dirShadowTarget;
		ref<RenderTarget> pointShadowTarget;

		MeshData* sphere;
		MeshData* tetrahedron;
		MeshData* cube;

		MeshData* plane;

	public:
		TestLayer();

		void SpawnCube(vector3 s = 1, float m = 1, bool locked = false);

		int Initialize() override;

		void PostUpdate() override;
		void FixedUpdate() override;

		void ImGui() override;
	};
}
