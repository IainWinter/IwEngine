#include "iw/renderer/Platform/OpenGL/GLPipeline.h"
#include "iw/renderer/Platform/OpenGL/GLPipelineParam.h"
#include "iw/renderer/Platform/OpenGL/GLUniformBuffer.h"
#include "iw/renderer/Platform/OpenGL/GLTranslator.h"
#include "gl/glew.h"

namespace iw {
namespace RenderAPI {
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

		int ucount = UniformCount();
		int bcount = BufferCount();

		const char* u = ucount == 1 ? " uniform" : " uniforms";
		const char* b = bcount == 1 ? " block"   : " blocks";

		LOG_DEBUG << "Shader #" << gl_id << " compiled with " << ucount << u << " & " << bcount << b << ":";

		for (int i = 0; i < ucount; i++) {
			IPipelineParam* uniform = GetParam(i);
			if (!uniform) {
				continue; // Check for null
			}
			
			LOG_DEBUG << "\tUniform Location: " << uniform->Location() << " Name: " << uniform->Name();
		}

 		for (int i = 0; i < bcount; i++) {
			//IPipelineParam* uniform = GetParam(i);
			//if (!uniform) {
			//	continue; // Check for null
			//}

			//LOG_DEBUG << "\tBuffer Location: " << uniform->Location() << " Name: " << uniform->Name();
		}
	}
	
	GLPipeline::~GLPipeline() {
		glDeleteProgram(gl_id);
		for (auto p : m_params) {
			delete p.second;
		}
	}

	unsigned GLPipeline::Handle() const {
		return gl_id;
	}

	int GLPipeline::UniformCount() const {
		int count;
		glGetProgramiv(gl_id, GL_ACTIVE_UNIFORMS, &count);
		return count;
	}

	int GLPipeline::BufferCount() const {
		int count;
		glGetProgramiv(gl_id, GL_ACTIVE_UNIFORM_BLOCKS, &count);
		return count;
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
			LOG_WARNING << "Couldn't find uniform " << name;
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
		int location = glGetUniformLocation(gl_id, name);

		if (location == -1) {
			return nullptr;
		}

		auto itr = m_params.find(name);
		if (itr != m_params.end()) {
			return itr->second;
		}

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
		GLPipelineParam* p = new GLPipelineParam(location, m_textureCount, s, type, typeSize, stride, count);

		return m_params.emplace(s, p).first->second;
	}

	void GLPipeline::SetBuffer(
		std::string name,
		IUniformBuffer* buffer)
	{
		int index = m_bufferCount++; // cant be bigger than GL_MAN_UNIFORM_BUFFER_COUNT
		
		dynamic_cast<GLBuffer*>(buffer)->BindBase(index);

		int uniformIndex = glGetUniformBlockIndex(gl_id, name.c_str());
#ifdef IW_DEBUG
		if (uniformIndex == -1) {
			LOG_WARNING << "Cannot find uniform buffer named " << name;
		}
#endif

		glUniformBlockBinding(gl_id, uniformIndex, index);
	}

	void GLPipeline::Use() {
		m_textureCount = 0;
		glUseProgram(gl_id);
	}
}
}
