#include "iw/renderer/Platform/OpenGL/GLFragmentShader.h"
#include "iw/log/logger.h"
#include "gl/glew.h"

namespace IW {
	GLFragmentShader::GLFragmentShader(
		const char* source)
	{
		gl_id = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(gl_id, 1, &source, nullptr);
		glCompileShader(gl_id);

		int result;
		glGetShaderiv(gl_id, GL_COMPILE_STATUS, &result);
		if (!result) {
			int length;
			glGetShaderiv(gl_id, GL_INFO_LOG_LENGTH, &length);

			char* message = new char[length];
			glGetShaderInfoLog(gl_id, length, &length, message);

			LOG_ERROR << "Error compiling fragment shader " << message;

			delete[] message;
			glDeleteShader(gl_id);
		}
	}

	unsigned GLFragmentShader::FragmentShader() const {
		return gl_id;
	}
}
