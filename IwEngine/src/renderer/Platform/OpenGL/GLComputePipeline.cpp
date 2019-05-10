#include "iw/renderer/Platform/OpenGL/GLComputePipeline.h"
#include "iw/renderer/Platform/OpenGL/GLPipelineParam.h"
#include "gl/glew.h"

namespace IwRenderer {
	GLComputePipeline::GLComputePipeline(
		GLComputeShader* computeShader)
	{
		m_program = glCreateProgram();
		glAttachShader(m_program, computeShader->ComputeShader());
		glLinkProgram(m_program);
		glValidateProgram(m_program);
	}

	GLComputePipeline::~GLComputePipeline() {
		glDeleteProgram(m_program);
	}

	IPipelineParam* GLComputePipeline::GetParam(
		const char* name)
	{
		int location = glGetUniformLocation(m_program, name);
		if (location != -1) {
			if (m_params.contains(location)) {
				return m_params.at(location);
			}

			else {
				GLPipelineParam* param = new GLPipelineParam(location);
				m_params.emplace(location, param);
				return param;
			}
		}

		return nullptr;
	}

	void GLComputePipeline::DispatchComputeShader(
		int x,
		int y,
		int z)
	{
		glDispatchCompute(x, y, z);
	}

	void GLComputePipeline::Use() const {
		glUseProgram(m_program);
	}
}
