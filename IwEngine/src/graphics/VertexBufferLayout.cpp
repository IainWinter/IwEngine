#include "iw/graphics/VertexBufferLayout.h"

namespace IwGraphics {
	VertexBufferLayout::VertexBufferLayout()
		: m_stride(0)
	{}

	unsigned int VertexBufferLayoutElement::GetSizeOfType(
		unsigned int glType)
	{
		switch (glType) {
		case GL_FLOAT:         return sizeof(GLfloat);
		case GL_UNSIGNED_INT:  return sizeof(GLuint);
		case GL_UNSIGNED_BYTE: return sizeof(GLbyte);
		}

		return 0;
	}
}

