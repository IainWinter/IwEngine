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
		iwm::vector3* Vertices;
		iwm::vector2* Uvs;
		iwm::vector3* Normals;
		unsigned int* Faces;
		Material* Materials;

		Object(
			const char* path)
			: Resource(path)
		{}
	};
}
