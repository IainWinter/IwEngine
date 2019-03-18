#pragma once

#include "IwGraphics.h"
#include <string>

namespace IwGraphics {
	struct ShaderProgramSource;
	class IWGRAPHICS_API ShaderProgram {
	private:
		unsigned int m_renderId;

	public:
		ShaderProgram() = default;

		ShaderProgram(
			const char* fileName);

		~ShaderProgram();

		void Use();
		void Delete();
	private:
		ShaderProgramSource ReadShader(
			const char* fileName);

		unsigned int CompileShader(
			unsigned int type,
			std::string source);
	};

	struct ShaderProgramSource {
		std::string Vertex;
		std::string Fragment;
	};
}
