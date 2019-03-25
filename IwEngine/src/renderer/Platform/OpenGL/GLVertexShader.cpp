#include "iw/renderer/Platform/OpenGL/GLVertexShader.h"
#include "iw/log/logger.h"
#include "gl/glew.h"

namespace IwRenderer {
	GLVertexShader::GLVertexShader(
		const char* source)
	{
		LOG_INFO << "Loaded vertex shader...\n" << source;

		m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(m_vertexShader, 1, &source, nullptr);
		glCompileShader(m_vertexShader);

		int result;
		glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &result);
		if (!result) {
			int length;
			glGetShaderiv(m_vertexShader, GL_INFO_LOG_LENGTH, &length);

			char* message = new char[length];
			glGetShaderInfoLog(m_vertexShader, length, &length, message);

			LOG_ERROR << "Error compiling vertex shader " << message;

			delete[] message;
			glDeleteShader(m_vertexShader);
		}
	}

	GLVertexShader::~GLVertexShader() {}
}
