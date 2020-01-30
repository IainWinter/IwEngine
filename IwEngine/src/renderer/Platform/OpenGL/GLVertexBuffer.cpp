#include "iw/renderer/Platform/OpenGL/GLVertexBuffer.h"
#include "gl/glew.h"

namespace IW {
	GLVertexBuffer::GLVertexBuffer(
		const void* data, 
		size_t size, 
		BufferIOType io)
		: GLBuffer(VERTEX, io, data, size)
	{}
}
