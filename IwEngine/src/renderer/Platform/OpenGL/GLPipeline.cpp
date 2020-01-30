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
		gl_id = glCreateProgram();
		glAttachShader(gl_id, vertexShader->VertexShader());
		glAttachShader(gl_id, fragmentShader->FragmentShader());
		if (geometryShader) {
			glAttachShader(gl_id, geometryShader->GeometryShader());
		}

		glLinkProgram(gl_id);
		glValidateProgram(gl_id);

		glDeleteShader(vertexShader->VertexShader());
		glDeleteShader(fragmentShader->FragmentShader());
		if(geometryShader) {
			glDeleteShader(geometryShader->GeometryShader());
		}
	}
	
	GLPipeline::~GLPipeline() {
		glDeleteProgram(gl_id);
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
			int location = glGetUniformLocation(gl_id, name.c_str());

			if (location != -1) {
				param = (GLPipelineParam*)GetParam(location);
				m_params.emplace(name, param);
			}

			else {
				LOG_WARNING << "Couldn't find uniform " << name;
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
		glGetActiveUniform(gl_id, index, 128, &nameSize, &count, &gltype, name);

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
			GLPipelineParam* p = new GLPipelineParam(index, m_textureCount, s, type, typeSize, stride, count);

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
		
		dynamic_cast<GLBuffer*>(buffer)->BindBase(index);

		int uniformIndex = glGetUniformBlockIndex(gl_id, name.c_str());
		glUniformBlockBinding(gl_id, uniformIndex, index);
	}

	int GLPipeline::UniformCount() {
		int count;
		glGetProgramiv(gl_id, GL_ACTIVE_UNIFORMS, &count);
		return count;
	}

	void GLPipeline::Use() {
		m_textureCount = 0;
		glUseProgram(gl_id);
	}
}
