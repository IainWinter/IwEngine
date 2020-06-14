#include "iw/renderer/Platform/OpenGL/GLFragmentShader.h"
#include "iw/renderer/Platform/OpenGL/GLErrorCatch.h"
#include "iw/log/logger.h"
#include "gl/glew.h"

namespace iw {
namespace RenderAPI {
	GLFragmentShader::GLFragmentShader(
		const char* source)
	{
		GL(gl_id = glCreateShader(GL_FRAGMENT_SHADER));
		GL(glShaderSource(gl_id, 1, &source, nullptr));
		GL(glCompileShader(gl_id));

		int result;
		GL(glGetShaderiv(gl_id, GL_COMPILE_STATUS, &result));
		if (!result) {
			int length;
			GL(glGetShaderiv(gl_id, GL_INFO_LOG_LENGTH, &length));

			char* message = new char[length];
			GL(glGetShaderInfoLog(gl_id, length, &length, message));

			LOG_ERROR << "Error compiling fragment shader " << message;

			delete[] message;
			GL(glDeleteShader(gl_id));
		}
	}

	unsigned GLFragmentShader::FragmentShader() const {
		return gl_id;
	}
}
}
