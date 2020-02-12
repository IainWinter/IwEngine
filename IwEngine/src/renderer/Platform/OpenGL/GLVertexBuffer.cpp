#include "iw/renderer/Platform/OpenGL/GLVertexBuffer.h"
#include "gl/glew.h"

namespace iw {
namespace RenderAPI {
	GLVertexBuffer::GLVertexBuffer(
		const void* data, 
		size_t size, 
		BufferIOType io)
		: GLBuffer(VERTEX, io, data, size)
	{}
}
}
