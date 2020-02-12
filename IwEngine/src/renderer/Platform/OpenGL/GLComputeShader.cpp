#include "iw/renderer/Platform/OpenGL/GlComputeShader.h"
#include "iw/log/logger.h"
#include "gl/glew.h"

namespace iw {
namespace RenderAPI {
	GLComputeShader::GLComputeShader(
		const char* source)
	{
		LOG_INFO << "Loaded fragment shader...\n" << source;

		gl_id = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(gl_id, 1, &source, nullptr);
		glCompileShader(gl_id);

		int result;
		glGetShaderiv(gl_id, GL_COMPILE_STATUS, &result);
		if (!result) {
			int length;
			glGetShaderiv(gl_id, GL_INFO_LOG_LENGTH, &length);

			char* message = new char[length];
			glGetShaderInfoLog(gl_id, length, &length, message);

			LOG_ERROR << "Error compiling compute shader " << message;

			delete[] message;
			glDeleteShader(gl_id);
		}
	}

	unsigned GLComputeShader::ComputeShader() const {
		return gl_id;
	}
}
}
