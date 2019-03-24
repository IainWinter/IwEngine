#include "iw/renderer/Platform/OpenGL/GLPipelineParam.h"
#include "gl/glew.h"

namespace IwRenderer {
	GLPipelineParam::GLPipelineParam(
		int location) 
		: m_location(location)
	{}

	void GLPipelineParam::SetAsMat4(
		const iwm::matrix4& matrix)
	{
		glUniformMatrix4fv(m_location, 1, GL_FALSE, matrix.elements);
	}
}
