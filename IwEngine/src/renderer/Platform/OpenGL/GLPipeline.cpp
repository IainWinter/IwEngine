#include "iw/renderer/Platform/OpenGL/GLPipeline.h"
#include "iw/renderer/Platform/OpenGL/GLPipelineParam.h"
#include "gl/glew.h"

namespace IW {
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
		GLPipelineParam* param = nullptr;

		auto itr = m_params.find(name);
		if (itr != m_params.end()) {
			param = itr->second;
		}

		else {
			int location = glGetUniformLocation(m_program, name);
			if (location != -1) {
				param = new GLPipelineParam(location);
				m_params.emplace(name, param);
			}
		}

		return param;
	}

	void GLPipeline::Use() const {
		glUseProgram(m_program);
	}
}
