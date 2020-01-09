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
				param = (GLPipelineParam*)GetParam(location);
				m_params.emplace(name, param);
			}
		}

		return param;
	}

	IPipelineParam* GLPipeline::GetParam(
		int index)
	{
		int nameSize = 128;
		int nameSizeActual;
		int      size;
		unsigned gltype;
		char     name[128];

		glGetActiveUniform(m_programId, index, nameSize, &nameSizeActual, &size, &gltype, name);

		UniformType type = (UniformType)TRANSLATE(gltype);
		unsigned typeSize = 0;
		switch (type) {
			case UniformType::BOOL:    typeSize = sizeof(bool);     break;
			case UniformType::INT:     typeSize = sizeof(int);      break;
			case UniformType::UINT:    typeSize = sizeof(unsigned); break;
			case UniformType::FLOAT:   typeSize = sizeof(float);    break;
			case UniformType::DOUBLE:  typeSize = sizeof(double);   break;
			case UniformType::SAMPLE2: typeSize = 0;                break;
		}

		return new GLPipelineParam(index, m_textureCount, type, typeSize, size, name);
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

	int GLPipeline::UniformCount() {
		int count;
		glGetProgramiv(m_programId, GL_ACTIVE_ATTRIBUTES, &count);
		return count;
	}

	void GLPipeline::Use() {
		m_textureCount = 0;
		glUseProgram(m_programId);
	}
}
