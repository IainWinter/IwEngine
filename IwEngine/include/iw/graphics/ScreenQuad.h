#pragma once

#include "iw/graphics/MeshFactory.h"

namespace iw {
namespace Graphics
{
	inline Mesh ScreenQuad() {
		static Mesh quad;

		if (!quad.Data) {
			MeshDescription description;
			description.DescribeBuffer(bName::POSITION, MakeLayout<float>(3));
			description.DescribeBuffer(bName::UV,       MakeLayout<float>(2));

			MeshData* data = MakePlane(description, 1, 1);
			data->TransformMeshData(Transform(glm::vec3(), glm::vec3(1), glm::quat(glm::vec3(-glm::pi<float>() * 0.5f, 0, 0))));

			quad = data->MakeInstance();
		}

		return quad;
	}

	using namespace Graphics;
}
}
