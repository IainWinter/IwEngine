#include "iw/renderer/ShaderProgram.h"
#include "iw/util/io/File.h"
#include "iw/log/logger.h"
#include "gl/glew.h"
#include <sstream>
#include <vector>

namespace IwRenderer {
	ShaderProgram::ShaderProgram(
		const char* fileName)
	{
		ShaderProgramSource source = ReadShader(fileName);

		m_renderId = glCreateProgram();
		unsigned int vsid = CompileShader(GL_VERTEX_SHADER, source.Vertex);
		unsigned int fsid = CompileShader(GL_FRAGMENT_SHADER, source.Fragment);

		glAttachShader(m_renderId, vsid);
		glAttachShader(m_renderId, fsid);
		glLinkProgram(m_renderId);
		glValidateProgram(m_renderId);

		glDeleteShader(vsid);
		glDeleteShader(fsid);
	}

	ShaderProgram::~ShaderProgram() {
		Delete();
	}

	ShaderProgramSource ShaderProgram::ReadShader(
		const char* fileName)
	{
		std::vector<std::string> code = IwUtil::ReadFileLines(fileName);
		std::stringstream ss[2];
		enum ShaderMode {
			NONE = -1,
			VERTEX,
			FRAGMENT
		};

		ShaderMode mode;
		for (std::string& line : code) {
			if (line.find("#shader") != std::string::npos) {
				if (line.find("vertex") != std::string::npos) {
					mode = VERTEX;
				}

				else if (line.find("fragment") != std::string::npos) {
					mode = FRAGMENT;
				}
			}
			else {
				ss[mode] << line << std::endl;
			}
		}

		return ShaderProgramSource { 
			ss[VERTEX].str(), 
			ss[FRAGMENT].str() 
		};
	}

	unsigned int ShaderProgram::CompileShader(
		unsigned int type,
		std::string source) 
	{
		unsigned int id = glCreateShader(type);
		const char* src = source.c_str();
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);

		int result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (!result) {
			int length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

			char* message = (char*)_malloca(length * sizeof(char));
			glGetShaderInfoLog(id, length, &length, message);

			LOG_ERROR << "Error compiling shader " << message;

			glDeleteShader(id);

			return 0;
		}

		return id;
	}

	void ShaderProgram::Use() const {
		glUseProgram(m_renderId);
	}

	void ShaderProgram::Delete() {
		glDeleteProgram(m_renderId);
	}
}
