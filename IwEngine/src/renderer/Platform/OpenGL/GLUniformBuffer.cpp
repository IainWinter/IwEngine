#include "iw/renderer/Platform/OpenGL/GLUniformBuffer.h"
#include "gl/glew.h"
#include <cstring>

namespace iw {
namespace RenderAPI {
	GLUniformBuffer::GLUniformBuffer(
		const void* data,
		size_t size,
		BufferIOType io)
		: GLBuffer(UNIFORM, io, data, size)
	{}
}
}
