#pragma once

#include "iw/math/vector2.h"
#include "iw/math/vector3.h"

namespace IwEngine {
	struct Vertex {
		iwm::vector3 Vertex;
		iwm::vector3 Normal;
		//iwm::vector2 TextureCoord;
	};

	struct Mesh {
		Vertex* Vertices;
		unsigned int VertexCount;
	};

	struct FaceIndex {
		unsigned int* Faces;
		unsigned int FaceCount;
	};

	struct Model {
		Mesh* Meshes;
		FaceIndex* Indices;
		unsigned int MeshCount;
	};
}
