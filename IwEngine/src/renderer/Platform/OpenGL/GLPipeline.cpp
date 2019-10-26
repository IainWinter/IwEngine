#include "iw/renderer/Platform/OpenGL/GLPipeline.h"
#include "iw/renderer/Platform/OpenGL/GLPipelineParam.h"
#include "iw/renderer/Platform/OpenGL/GLUniformBuffer.h"
#include "gl/glew.h"

namespace IW {
	GLPipeline::GLPipeline(
		GLVertexShader* vertexShader,
		GLFragmentShader* fragmentShader,
		GLGeometryShader* geometryShader)
		: m_bufferCount(0)
	{
		m_programId = glCreateProgram();
		glAttachShader(m_programId, vertexShader->VertexShader());
		glAttachShader(m_programId, fragmentShader->FragmentShader());
		if (geometryShader) {
			glAttachShader(m_programId, geometryShader->GeometryShader());
		}

		glLinkProgram(m_programId);
		glValidateProgram(m_programId);

		glDeleteShader(vertexShader->VertexShader());
		glDeleteShader(fragmentShader->FragmentShader());
		if(geometryShader) {
			glDeleteShader(geometryShader->GeometryShader());
		}
	}
	
	GLPipeline::~GLPipeline() {
		glDeleteProgram(m_programId);
		for (auto p : m_params) {
			delete p.second;
		}
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
			int location = glGetUniformLocation(m_programId, name);
			
			if (location != -1) {
				param = new GLPipelineParam(location);
				m_params.emplace(name, param);
			}
		}

		return param;
	}

	void GLPipeline::SetBuffer(
		const char* name,
		IUniformBuffer* buffer)
	{
		int index = m_bufferCount++; // cant be bigger than GL_MAN_UNIFORM_BUFFER_COUNT
		
		((GLUniformBuffer*)buffer)->BindBase(index);

		int uniformIndex = glGetUniformBlockIndex(m_programId, name);
		glUniformBlockBinding(m_programId, uniformIndex, index);
	}

	void GLPipeline::Use() const {
		glUseProgram(m_programId);
	}
}
