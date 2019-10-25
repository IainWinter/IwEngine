#include "iw/renderer/Platform/OpenGL/GLFragmentShader.h"
#include "iw/log/logger.h"
#include "gl/glew.h"

namespace IW {
	GLFragmentShader::GLFragmentShader(
		const char* source)
	{
		LOG_INFO << "Loaded fragment shader...\n" << source;

		m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(m_fragmentShader, 1, &source, nullptr);
		glCompileShader(m_fragmentShader);

		int result;
		glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &result);
		if (!result) {
			int length;
			glGetShaderiv(m_fragmentShader, GL_INFO_LOG_LENGTH, &length);

			char* message = new char[length];
			glGetShaderInfoLog(m_fragmentShader, length, &length, message);

			LOG_ERROR << "Error compiling fragment shader " << message;

			delete[] message;
			glDeleteShader(m_fragmentShader);
		}
	}

	GLFragmentShader::~GLFragmentShader() {}
}
