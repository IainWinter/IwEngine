#include "iw/renderer/Platform/OpenGL/GLGeometryShader.h"
#include "iw/log/logger.h"
#include "gl/glew.h"

namespace IW {
	GLGeometryShader::GLGeometryShader(
		const char* source)
	{
		m_geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(m_geometryShader, 1, &source, nullptr);
		glCompileShader(m_geometryShader);

		int result;
		glGetShaderiv(m_geometryShader, GL_COMPILE_STATUS, &result);
		if (!result) {
			int length;
			glGetShaderiv(m_geometryShader, GL_INFO_LOG_LENGTH, &length);

			char* message = new char[length];
			glGetShaderInfoLog(m_geometryShader, length, &length, message);

			LOG_ERROR << "Error compiling geometry shader " << message;

			delete[] message;
			glDeleteShader(m_geometryShader);
		}
	}

	GLGeometryShader::~GLGeometryShader() {}
}
