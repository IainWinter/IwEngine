#include "iw/renderer/Platform/OpenGL/GLPipeline.h"
#include "iw/renderer/Platform/OpenGL/GLPipelineParam.h"
#include "iw/renderer/Platform/OpenGL/GLUniformBuffer.h"
#include "iw/renderer/Platform/OpenGL/GLTranslator.h"
#include "iw/renderer/Platform/OpenGL/GLErrorCatch.h"
#include "gl/glew.h"

namespace iw {
namespace RenderAPI {
	GLPipeline::GLPipeline(
		GLVertexShader* vertexShader,
		GLFragmentShader* fragmentShader,
		GLGeometryShader* geometryShader)
		: m_bufferCount(0)
		, m_textureCount(0)
		, m_imageCount(0)
	{
		gl_id = glCreateProgram();

		if (vertexShader) {
			GL(glAttachShader(gl_id, vertexShader->VertexShader()));
		}

		if (fragmentShader) {
			GL(glAttachShader(gl_id, fragmentShader->FragmentShader()));
		}

		if (geometryShader) {
			GL(glAttachShader(gl_id, geometryShader->GeometryShader()));
		}

		GL(glLinkProgram(gl_id));
		GL(glValidateProgram(gl_id));

		if (vertexShader) {
			GL(glDeleteShader(vertexShader->VertexShader()));
		}

		if (fragmentShader) {
			GL(glDeleteShader(fragmentShader->FragmentShader()));
		}
		
		if(geometryShader) {
			GL(glDeleteShader(geometryShader->GeometryShader()));
		}

		int ucount = UniformCount();
		int bcount = BufferCount();

		const char* u = ucount == 1 ? " uniform" : " uniforms";
		const char* b = bcount == 1 ? " block"   : " blocks";

		LOG_DEBUG << "Shader #" << gl_id << " compiled with " << ucount << u << " & " << bcount << b << ":";

		for (int i = 0; i < ucount; i++) {
			IPipelineParam* uniform = GetParam(i); // scans all uniforms
			if (!uniform) {
				continue;
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

	GLPipeline::GLPipeline(
		GLComputeShader* computeShader)
	{
		gl_id = glCreateProgram();

		GL(glAttachShader(gl_id, computeShader->ComputeShader()));
		GL(glLinkProgram(gl_id));
		GL(glValidateProgram(gl_id));
		GL(glDeleteShader(computeShader->ComputeShader()));

		int ucount = UniformCount();
		int bcount = BufferCount();

		const char* u = ucount == 1 ? " uniform" : " uniforms";
		const char* b = bcount == 1 ? " block" : " blocks";

		LOG_DEBUG << "Shader #" << gl_id << " compiled with " << ucount << u << " & " << bcount << b << ":";

		for (int i = 0; i < ucount; i++) {
			IPipelineParam* uniform = GetParam(i); // scans all uniforms
			if (!uniform) {
				continue;
			}

			LOG_DEBUG << "\tUniform Location: " << uniform->Location() << " Name: " << uniform->Name();
		}
	}
	
	GLPipeline::~GLPipeline() {
		GL(glDeleteProgram(gl_id));
		for (auto p : m_params) {
			delete p.second;
		}
	}

	unsigned GLPipeline::Handle() const {
		return gl_id;
	}

	int GLPipeline::UniformCount() const {
		int count;
		GL(glGetProgramiv(gl_id, GL_ACTIVE_UNIFORMS, &count));
		return count;
	}

	int GLPipeline::BufferCount() const {
		int count;
		GL(glGetProgramiv(gl_id, GL_ACTIVE_UNIFORM_BLOCKS, &count));
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
			//LOG_WARNING << "Couldn't find uniform " << name; // make has param function and uncomment this
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

		GL(glGetActiveUniform(gl_id, index, 128, &nameSize, &count, &gltype, name));
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
		GLPipelineParam* p = new GLPipelineParam(location, m_textureCount, m_imageCount, s, type, typeSize, stride, count);

		return m_params.emplace(s, p).first->second;
	}

	void GLPipeline::SetBuffer(
		std::string name,
		IUniformBuffer* buffer)
	{
		//unsigned index = m_bufferCount++; // cant be bigger than GL_MAN_UNIFORM_BUFFER_COUNT

		buffer->BindMyBase();

		int uniformIndex = glGetUniformBlockIndex(gl_id, name.c_str());
		if (uniformIndex == -1) {
			LOG_WARNING << "Cannot find uniform buffer named " << name;
			return;
		}

		GL(glUniformBlockBinding(gl_id, uniformIndex, buffer->MyBase()));
	}

	void GLPipeline::GetComputeWorkGroupSize(
		int& x, 
		int& y, 
		int& z) const
	{
		int buf[3];
		glGetProgramiv(gl_id, GL_COMPUTE_WORK_GROUP_SIZE, buf);
		x = buf[0];
		y = buf[1];
		z = buf[2];
	}

	void GLPipeline::DispatchCompute(
		int x,
		int y,
		int z) const
	{
		glDispatchCompute(x, y, z);
	}

//	bool GLPipeline::IsTextureActive(
//		IPipelineParam* param) const
//	{
//#ifdef IW_DEBUG
//		if (param->Type() != UniformType::SAMPLER) {
//			LOG_WARNING << "Checking for active texture but param is not a texture!";
//		}
//#endif 
//		GLint* texture = new GLint[4]; 
//		glGetIntegerv(GL_ACTIVE_TEXTURE, texture);
//
//		unsigned g = param->SetAsTexture();
//
//		//if (texture == param->Location()) {
//
//		//}
//
//		return false;
//	}

	void GLPipeline::Use() {
		m_textureCount = 0;
		m_imageCount   = 0;
		GL(glUseProgram(gl_id));
	}
}
}
