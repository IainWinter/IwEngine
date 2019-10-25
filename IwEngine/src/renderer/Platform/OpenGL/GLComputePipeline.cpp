#include "iw/renderer/Platform/OpenGL/GLComputePipeline.h"
#include "iw/renderer/Platform/OpenGL/GLPipelineParam.h"
#include "gl/glew.h"

namespace IW {
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
