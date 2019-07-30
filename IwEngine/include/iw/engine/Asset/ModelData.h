#pragma once

#include "iw/graphics/Vertex.h"

namespace IwEngine {
	struct MeshData {
		IwGraphics::Vertex* Vertices;
		unsigned int*       Faces;
		std::size_t         VertexCount;
		std::size_t         FaceCount;

		~MeshData() {
			delete[] Vertices;
			delete[] Faces;
		}
	};

	struct ModelData {
		MeshData*   Meshes;
		std::size_t MeshCount;

		~ModelData() {
			delete[] Meshes;
		}
	};
}
