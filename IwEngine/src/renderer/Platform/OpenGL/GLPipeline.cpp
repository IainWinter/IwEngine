#include "iw/renderer/Platform/OpenGL/GLPipeline.h"
#include "iw/renderer/Platform/OpenGL/GLPipelineParam.h"
#include "gl/glew.h"

namespace IwRenderer {
	GLPipeline::GLPipeline(
		GLVertexShader* vertexShader, 
		GLFragmentShader* fragmentShader,
		GLGeometryShader* geometryShader)
	{
		m_program = glCreateProgram();
		glAttachShader(m_program, vertexShader->VertexShader());
		glAttachShader(m_program, fragmentShader->FragmentShader());
		if (geometryShader) {
			glAttachShader(m_program, geometryShader->GeometryShader());
		}

		glLinkProgram(m_program);
		glValidateProgram(m_program);

		glDeleteShader(vertexShader->VertexShader());
		glDeleteShader(fragmentShader->FragmentShader());
		if(geometryShader) {
			glDeleteShader(geometryShader->GeometryShader());
		}
	}
	
	GLPipeline::~GLPipeline() {
		glDeleteProgram(m_program);
	}

	IPipelineParam* GLPipeline::GetParam(
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

	void GLPipeline::Use() const {
		glUseProgram(m_program);
	}
}
