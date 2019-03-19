#pragma once

#include "Resource.h"
#include "Mtl.h"
#include "iw/math/vector2.h"
#include "iw/math/vector3.h"
#include <vector>

namespace IwEngine {
	struct Obj
		: Resource
	{
		iwm::vector3* Vertices;
		iwm::vector2* Uvs;
		iwm::vector3* Normals;
		unsigned int* Faces;
		Mtl* Materials;

		Obj(
			const char* path)
			: Resource(path)
		{}
	};
}
