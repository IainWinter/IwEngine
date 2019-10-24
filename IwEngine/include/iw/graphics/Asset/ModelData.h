#pragma once

#include "iw/graphics/Vertex.h"

namespace IwGraphics {
	struct MeshData {
		iwm::vector3* Vertices;
		iwm::vector3* Normals;

		unsigned int* Faces;
		size_t VertexCount;
		size_t FaceCount;

		~MeshData() {
			delete[] Vertices;
			delete[] Normals;
			delete[] Faces;
		}
	};

	struct ModelData {
		MeshData* Meshes;
		size_t MeshCount;

		~ModelData() {
			delete[] Meshes;
		}
	};
}
