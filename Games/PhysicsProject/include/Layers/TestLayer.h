#pragma once 

#include "iw/engine/Layer.h"
#include "iw/graphics/Font.h"
#include "iw/graphics/PointLight.h"

#include "glm/vec3.hpp"

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

		Entity Ground;

	public:
		TestLayer();

		Entity SpawnCube(glm::vec3 s = glm::vec3(1), float m = 1, bool locked = false);

		void CorrectVelocity(Entity entity, Entity target, glm::vec3 offset);

		int Initialize() override;
		void FixedUpdate() override;
		void ImGui() override;
	};
}
