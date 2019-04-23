#pragma once

#include "iw/math/vector2.h"
#include "iw/math/vector3.h"

namespace IwGraphics {
	struct Vertex {
		iwm::vector3 Vertex;
		iwm::vector3 Normal;
		//iwm::vector2 TextureCoord;
	};

	struct MeshData {
		Vertex* Vertices;
		std::size_t VertexCount;
	};

	struct FaceIndex {
		unsigned int* Faces;
		std::size_t FaceCount;
	};

	struct ModelData {
		MeshData* Meshes;
		FaceIndex* Indices;
		std::size_t MeshCount;
	};
}
