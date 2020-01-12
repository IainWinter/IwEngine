#include "iw/renderer/Platform/OpenGL/GLPipeline.h"
#include "iw/renderer/Platform/OpenGL/GLPipelineParam.h"
#include "iw/renderer/Platform/OpenGL/GLUniformBuffer.h"
#include "iw/renderer/Platform/OpenGL/GLTranslator.h"
#include "gl/glew.h"

namespace IW {
	GLPipeline::GLPipeline(
		GLVertexShader* vertexShader,
		GLFragmentShader* fragmentShader,
		GLGeometryShader* geometryShader)
		: m_bufferCount(0)
		, m_textureCount(0)
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
		std::string name)
	{
		GLPipelineParam* param = nullptr;

		auto itr = m_params.find(name);
		if (itr != m_params.end()) {
			param = itr->second;
		}

		else {
			int location = glGetUniformLocation(m_programId, name.c_str());

			if (location != -1) {
				param = (GLPipelineParam*)GetParam(location);
				m_params.emplace(name, param);
			}
		}

		return param;
	}

	IPipelineParam* GLPipeline::GetParam(
		int index)
	{
		int      nameSize;
		int      count;
		unsigned gltype;
		char     name[128];

		memset(name, '\0', 128);
		glGetActiveUniform(m_programId, index, 128, &nameSize, &count, &gltype, name);

		auto itr = m_params.find(name);
		if (itr == m_params.end()) {
			unsigned typeSize = 0;
			unsigned stride = 0;
			UniformType type = (UniformType)TRANSLATE(gltype, stride);
			switch (type) {
				case UniformType::BOOL:    typeSize = sizeof(bool);     break;
				case UniformType::INT:     typeSize = sizeof(int);      break;
				case UniformType::UINT:    typeSize = sizeof(unsigned); break;
				case UniformType::FLOAT:   typeSize = sizeof(float);    break;
				case UniformType::DOUBLE:  typeSize = sizeof(double);   break;
			}

			std::string s(name);
			GLPipelineParam* p = new GLPipelineParam(index, m_textureCount, type, typeSize, stride, count, s);

			return m_params.emplace(s, p).first->second;
		}

		// todo: sus maybe mem leak this is a bad hack solve ^ should combine with function above

		return itr->second;
	}

	void GLPipeline::SetBuffer(
		std::string name,
		IUniformBuffer* buffer)
	{
		int index = m_bufferCount++; // cant be bigger than GL_MAN_UNIFORM_BUFFER_COUNT
		
		((GLUniformBuffer*)buffer)->BindBase(index);

		int uniformIndex = glGetUniformBlockIndex(m_programId, name.c_str());
		glUniformBlockBinding(m_programId, uniformIndex, index);
	}

	int GLPipeline::UniformCount() {
		int count;
		glGetProgramiv(m_programId, GL_ACTIVE_UNIFORMS, &count);
		return count;
	}

	void GLPipeline::Use() {
		m_textureCount = 0;
		glUseProgram(m_programId);
	}
}
