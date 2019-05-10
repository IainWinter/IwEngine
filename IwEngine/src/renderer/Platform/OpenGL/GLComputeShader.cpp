#include "iw/renderer/Platform/OpenGL/GlComputeShader.h"
#include "iw/log/logger.h"
#include "gl/glew.h"

namespace IwRenderer {
	GLComputeShader::GLComputeShader(
		const char* source)
	{
		LOG_INFO << "Loaded fragment shader...\n" << source;

		m_computeShader = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(m_computeShader, 1, &source, nullptr);
		glCompileShader(m_computeShader);

		int result;
		glGetShaderiv(m_computeShader, GL_COMPILE_STATUS, &result);
		if (!result) {
			int length;
			glGetShaderiv(m_computeShader, GL_INFO_LOG_LENGTH, &length);

			char* message = new char[length];
			glGetShaderInfoLog(m_computeShader, length, &length, message);

			LOG_ERROR << "Error compiling compute shader " << message;

			delete[] message;
			glDeleteShader(m_computeShader);
		}
	}

	GLComputeShader::~GLComputeShader() {}
}
