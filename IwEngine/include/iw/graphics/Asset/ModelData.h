#pragma once

#include "iw/graphics/Vertex.h"

namespace IwGraphics {
	struct MeshData {
		Vertex*       Vertices;
		unsigned int* Faces;
		size_t   VertexCount;
		size_t   FaceCount;

		~MeshData() {
			delete[] Vertices;
			delete[] Faces;
		}
	};

	struct ModelData {
		MeshData*   Meshes;
		size_t MeshCount;

		~ModelData() {
			delete[] Meshes;
		}
	};
}
