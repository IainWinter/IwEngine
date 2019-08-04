#pragma once

#include "iw/renderer/VertexArray.h"
#include "iw/renderer/IndexBuffer.h"
#include "iw/util/async/potential.h"
#include <cstddef>

namespace IwGraphics {
	struct Mesh {
		iwu::potential<IwRenderer::IVertexArray*> VertexArray;
		iwu::potential<IwRenderer::IIndexBuffer*> IndexBuffer;
		std::size_t FaceCount;
	};
}
