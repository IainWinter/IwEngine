#pragma once 

#include "iw/engine/Layer.h"
#include "iw/graphics/Font.h"
#include "iw/graphics/PointLight.h"

namespace iw {
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

		Entity Ball;
		Entity Ground;

	public:
		TestLayer();

		Entity SpawnCube(vector3 s = 1, float m = 1, bool locked = false);

		int Initialize() override;
		void FixedUpdate() override;
		void ImGui() override;
	};
}
