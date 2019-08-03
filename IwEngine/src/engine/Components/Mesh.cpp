#include "iw/engine/Components/Mesh.h"

namespace IwEngine {
	Mesh::~Mesh() {
		delete VertexArray;
		delete IndexBuffer;
	}
}
