#pragma once

#include "iw/renderer/VertexArray.h"
#include "iw/renderer/IndexBuffer.h"
#include "iw/graphics/Vertex.h"
#include <cstddef>

namespace IwEngine {
	struct Mesh {
		IwRenderer::IVertexArray* VertexArray;
		IwRenderer::IIndexBuffer* IndexBuffer;
		std::size_t               FaceCount;
	};
}
