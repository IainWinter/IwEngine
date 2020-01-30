#include "iw/renderer/Platform/OpenGL/GLUniformBuffer.h"
#include "gl/glew.h"
#include <cstring>

namespace IW {
	GLUniformBuffer::GLUniformBuffer(
		const void* data,
		size_t size,
		BufferIOType io)
		: GLBuffer(UNIFORM, io, data, size)
	{}
}
