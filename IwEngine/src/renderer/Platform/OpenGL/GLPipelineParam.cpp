#include "iw/renderer/Platform/OpenGL/GLPipelineParam.h"
#include "gl/glew.h"

namespace IW {
	GLPipelineParam::GLPipelineParam(
		int location) 
		: m_location(location)
	{}

	void GLPipelineParam::SetAsFloat(
		const float& val)
	{
		glUniform1f(m_location, val);
	}

	void GLPipelineParam::SetAsVec2(
		const iwm::vector2& vec)
	{
		glUniform2f(m_location, vec.x, vec.y);
	}

	void GLPipelineParam::SetAsVec3(
		const iwm::vector3& vec)
	{
		glUniform3f(m_location, vec.x, vec.y, vec.z);
	}

	void GLPipelineParam::SetAsVec4(
		const iwm::vector4& vec) 
	{
		glUniform4f(m_location, vec.x, vec.y, vec.z, vec.w);
	}

	void GLPipelineParam::SetAsMat2(
		const iwm::matrix2& matrix)
	{
		glUniformMatrix2fv(m_location, 1, GL_FALSE, matrix.elements);
	}

	void GLPipelineParam::SetAsMat3(
		const iwm::matrix3& matrix)
	{
		glUniformMatrix3fv(m_location, 1, GL_FALSE, matrix.elements);
	}

	void GLPipelineParam::SetAsMat4(
		const iwm::matrix4& matrix)
	{
		glUniformMatrix4fv(m_location, 1, GL_FALSE, matrix.elements);
	}
}
