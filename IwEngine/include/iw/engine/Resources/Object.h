#pragma once

#include "Resource.h"
#include "Material.h"
#include "iw/math/vector2.h"
#include "iw/math/vector3.h"
#include <vector>

namespace IwEngine {
	struct Object
		: Resource
	{
		std::vector<iwm::vector3> Vertices;
		std::vector<iwm::vector3> Normals;
		std::vector<iwm::vector3> Colors;
		std::vector<iwm::vector2> TexCoords;
		std::vector<unsigned int> Faces;
		Material* Materials;

		Object(
			const char* path)
			: Resource(path)
		{}
	};
}
