#include "iw/renderer/Platform/OpenGL/GLVertexShader.h"
#include "iw/renderer/Platform/OpenGL/GLErrorCatch.h"
#include "iw/log/logger.h"
#include "gl/glew.h"

namespace iw {
namespace RenderAPI {
	GLVertexShader::GLVertexShader(
		const char* source)
	{
		GL(gl_id = glCreateShader(GL_VERTEX_SHADER));
		GL(glShaderSource(gl_id, 1, &source, nullptr));
		GL(glCompileShader(gl_id));

		int result;
		GL(glGetShaderiv(gl_id, GL_COMPILE_STATUS, &result));
		if (!result) {
			int length;
			GL(glGetShaderiv(gl_id, GL_INFO_LOG_LENGTH, &length));

			char* message = new char[length];
			GL(glGetShaderInfoLog(gl_id, length, &length, message));

			LOG_ERROR << "Error compiling vertex shader " << message;

			delete[] message;
			GL(glDeleteShader(gl_id));
		}
	}

	unsigned GLVertexShader::VertexShader() const {
		return gl_id;
	}
}
}
