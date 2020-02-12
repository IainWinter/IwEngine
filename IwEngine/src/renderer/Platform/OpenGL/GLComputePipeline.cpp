#include "iw/renderer/Platform/OpenGL/GLComputePipeline.h"
#include "iw/renderer/Platform/OpenGL/GLPipelineParam.h"
#include "gl/glew.h"

namespace iw {
namespace RenderAPI {
	GLComputePipeline::GLComputePipeline(
		GLComputeShader* computeShader)
	{
		gl_id = glCreateProgram();
		glAttachShader(gl_id, computeShader->ComputeShader());
		glLinkProgram(gl_id);
		glValidateProgram(gl_id);
	}

	GLComputePipeline::~GLComputePipeline() {
		glDeleteProgram(gl_id);
	}

	void GLComputePipeline::DispatchComputeShader(
		int x,
		int y,
		int z)
	{
		glDispatchCompute(x, y, z);
	}

	void GLComputePipeline::Use() const {
		glUseProgram(gl_id);
	}
}
}
