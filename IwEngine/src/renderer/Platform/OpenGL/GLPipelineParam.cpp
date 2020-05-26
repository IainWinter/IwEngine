#include "iw/renderer/Platform/OpenGL/GLPipelineParam.h"
#include "iw/renderer/Platform/OpenGL/GLTexture.h"
#include "iw/renderer/Platform/OpenGL/GLErrorCatch.h"
#include "iw/renderer/Platform/OpenGL/GLTranslator.h"
#include "gl/glew.h"

namespace iw {
namespace RenderAPI {
	GLPipelineParam::GLPipelineParam(
		unsigned location,
		unsigned textureCount,
		unsigned imageCount,
		std::string name,
		UniformType type,
		unsigned typeSize,
		unsigned stride,
		unsigned count)
		: m_location(location)
		, m_textureCount(textureCount)
		, m_imageCount(imageCount)
		, m_name(name)
		, m_type(type)
		, m_typeSize(typeSize)
		, m_stride(stride)
		, m_count(count)
	{
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, (GLint*)&MAX_TEXTURES);
		glGetIntegerv(GL_MAX_IMAGE_UNITS, (GLint*)&MAX_IMAGES);
	}

	const std::string& GLPipelineParam::Name() const {
		return m_name;
	}

	unsigned GLPipelineParam::Location() const {
		return m_location;
	}

	UniformType GLPipelineParam::Type() const {
		return m_type;
	}

	unsigned GLPipelineParam::TypeSize() const {
		return m_typeSize;
	}

	unsigned GLPipelineParam::Stride() const {
		return m_stride;
	}

	unsigned GLPipelineParam::Count() const {
		return m_count;
	}

#define SET_AS(t, gt, n, sn)                                                               \
	void GLPipelineParam::SetAs##n(                                                        \
		t value)                                                                           \
	{                                                                                      \
		SetAs##n##s(&value, 1, 1);                                                         \
	}                                                                                      \
		                                                                                   \
	void GLPipelineParam::SetAs##n##s(                                                     \
		const void* values,                                                                \
		unsigned stride,                                                                   \
		unsigned count)                                                                    \
	{                                                                                      \
		bool err = false;                                                                  \
		const gt* v = (const gt*)values;                                                   \
		/*if (count == 1) {                                                                \
			switch (stride) {                                                              \
				case 1: GLe(glUniform1##sn(m_location, v[0]));                   break;    \
				case 2: GLe(glUniform2##sn(m_location, v[0], v[1]));             break;    \
				case 3: GLe(glUniform3##sn(m_location, v[0], v[1], v[2]));       break;    \
				case 4: GLe(glUniform4##sn(m_location, v[0], v[1], v[2], v[3])); break;    \
				default: LOG_WARNING << "Stride is too large (" << (stride) << ")"; break; \
			}                                                                              \
		}*/                                                                                \
		                                                                                   \
		/*else {*/                                                                         \
			switch (stride) {                                                              \
				case 1: GLe(glUniform1##sn##v(m_location, count, v)); break;               \
				case 2: GLe(glUniform2##sn##v(m_location, count, v)); break;               \
				case 3: GLe(glUniform3##sn##v(m_location, count, v)); break;               \
				case 4: GLe(glUniform4##sn##v(m_location, count, v)); break;               \
				default: LOG_WARNING << "Stride is too large (" << (stride) << ")"; break; \
			}                                                                              \
		/*}*/                                                                              \
                                                                                           \
        if(err) {                                                                          \
			LOG_WARNING << "Problem setting uniform " << m_name;                           \
		}                                                                                  \
	}                                                                                      \

	SET_AS(int,      GLint,    Int,    i)
	SET_AS(unsigned, GLuint,   UInt,  ui)
	SET_AS(float,    GLfloat,  Float,  f)
	SET_AS(double,   GLdouble, Double, d)

#undef SET_AS

	void GLPipelineParam::SetAsBool(
		bool value)
	{
		SetAsInt(value);
	}

	void GLPipelineParam::SetAsBools(
		const void* values,
		unsigned count,
		unsigned stride)
	{
		SetAsInts(values, count, stride);
	}

	/*void GLPipelineParam::SetAsInt(
		int value)
	{
		SetAsInts(&value, 1, 1);
		glUniform1i(m_location, value);
	}

	void GLPipelineParam::SetAsInts(
		const void* values,
		unsigned stride,
		unsigned count)
	{
		if (count != 1) {
			switch (stride) {
				case 1: glUniform1iv(m_location, count, (const GLint*)values); break;
				case 2: glUniform2iv(m_location, count, (const GLint*)values); break;
				case 3: glUniform3iv(m_location, count, (const GLint*)values); break;
				case 4: glUniform4iv(m_location, count, (const GLint*)values); break;
				default: LOG_WARNING << "Stride is too large (" << (stride) << ")"; break;
			}
		}

		else {
			const GLint* v = (const GLint*)values;
			switch (stride) {
				case 1: glUniform1i(m_location, v[0]); break;
				case 2: glUniform2i(m_location, v[0], v[1]); break;
				case 3: glUniform3i(m_location, v[0], v[1], v[2]); break;
				case 4: glUniform4i(m_location, v[0], v[1], v[2], v[3]); break;
				default: LOG_WARNING << "Stride is too large (" << (stride) << ")"; break;
			}
		}
	}

	void GLPipelineParam::SetAsUInt(
		unsigned int value)
	{
		glUniform1ui(m_location, value);
	}

	void GLPipelineParam::SetAsUInts(
		const void* values,
		unsigned count,
		unsigned stride)
	{
		if (stride) {
			switch (stride) {
				case 1: glUniform1uiv(m_location, count, (const GLuint*)values); break;
				case 2: glUniform2uiv(m_location, count, (const GLuint*)values); break;
				case 3: glUniform3uiv(m_location, count, (const GLuint*)values); break;
				case 4: glUniform4uiv(m_location, count, (const GLuint*)values); break;
				default: LOG_WARNING << "Stride is too large (" << (stride) << ")"; break;
			}
		}

		else {
			const GLuint* v = (const GLuint*)values;
			switch (count) {
				case 1: glUniform1ui(m_location, v[0]); break;
				case 2: glUniform2ui(m_location, v[0], v[1]); break;
				case 3: glUniform3ui(m_location, v[0], v[1], v[2]); break;
				case 4: glUniform4ui(m_location, v[0], v[1], v[2], v[3]); break;
				default: LOG_WARNING << "Stride is too large (" << (stride) << ")"; break;
			}
		}
	}

	void GLPipelineParam::SetAsFloat(
		float value)
	{
		glUniform1f(m_location, value);
	}

	void GLPipelineParam::SetAsFloats(
		const void* values,
		unsigned count,
		unsigned stride)
	{
		if (stride) {
			switch (stride) {
				case 1: glUniform1fv(m_location, count, (const GLfloat*)values); break;
				case 2: glUniform2fv(m_location, count, (const GLfloat*)values); break;
				case 3: glUniform3fv(m_location, count, (const GLfloat*)values); break;
				case 4: glUniform4fv(m_location, count, (const GLfloat*)values); break;
				default: LOG_WARNING << "Stride is too large (" << (stride) << ")";  break;
			}
		}

		else {
			const GLfloat* v = (const GLfloat*)values;
			switch (count) {
				case 1: glUniform1f(m_location, v[0]); break;
				case 2: glUniform2f(m_location, v[0], v[1]); break;
				case 3: glUniform3f(m_location, v[0], v[1], v[2]); break;
				case 4: glUniform4f(m_location, v[0], v[1], v[2], v[3]); break;
				default: LOG_WARNING << "Stride is too large (" << (stride) << ")"; break;
			}
		}
	}

	void GLPipelineParam::SetAsDouble(
		double value)
	{
		glUniform1d(m_location, value);
	}

	void GLPipelineParam::SetAsDoubles(
		const void* values,
		unsigned count,
		unsigned stride)
	{
		if (stride) {
			switch (stride) {
				case 1: glUniform1dv(m_location, count, (const GLdouble*)values); break;
				case 2: glUniform2dv(m_location, count, (const GLdouble*)values); break;
				case 3: glUniform3dv(m_location, count, (const GLdouble*)values); break;
				case 4: glUniform4dv(m_location, count, (const GLdouble*)values); break;
				default: LOG_WARNING << "Stride is too large (" << (stride) << ")"; break;
			}
		}

		else {
			const GLdouble* v = (const GLdouble*)values;
			switch (count) {
				case 1: glUniform1d(m_location, v[0]); break;
				case 2: glUniform2d(m_location, v[0], v[1]); break;
				case 3: glUniform3d(m_location, v[0], v[1], v[2]); break;
				case 4: glUniform4d(m_location, v[0], v[1], v[2], v[3]); break;
				default: LOG_WARNING << "Stride is too large (" << (stride) << ")"; break;
			}
		}
	}*/

	void GLPipelineParam::SetAsMat2s(
		const void* matrix,
		int count)
	{
		bool err = false;

		GLe(glUniformMatrix2fv(m_location, count, GL_FALSE, (float*)matrix));

		if (err) {
			LOG_WARNING << "Problem setting uniform " << m_name;
		}
	}

	void GLPipelineParam::SetAsMat3s(
		const void* matrix,
		int count)
	{
		bool err = false;

		GLe(glUniformMatrix3fv(m_location, count, GL_FALSE, (float*)matrix));

		if (err) {
			LOG_WARNING << "Problem setting uniform " << m_name;
		}
	}

	void GLPipelineParam::SetAsMat4s(
		const void* matrix,
		int count)
	{
		bool err = false;

		GLe(glUniformMatrix4fv(m_location, count, GL_FALSE, (float*)matrix));

		if (err) {
			LOG_WARNING << "Problem setting uniform " << m_name;
		}
	}

	void GLPipelineParam::SetAsMat2(
		const iw::matrix2& matrix)
	{
		SetAsMat2s(&matrix);
	}

	void GLPipelineParam::SetAsMat3(
		const iw::matrix3& matrix)
	{
		SetAsMat3s(&matrix);
	}

	void GLPipelineParam::SetAsMat4(
		const iw::matrix4& matrix)
	{
		SetAsMat4s(&matrix);
	}

	void GLPipelineParam::SetAsTexture(
		const ITexture* texture,
		int index)
	{
		if (index < 0) {
			index = m_textureCount;
		}

#ifdef IW_DEBUG
		if (index > MAX_TEXTURES) {
			LOG_WARNING << "Cannot bind more than " << MAX_TEXTURES << " textures!";
		}
#endif

		glUniform1i(m_location, index);
		glActiveTexture(GL_TEXTURE0 + index);

		if (texture) {
			static_cast<const GLTexture*>(texture)->Bind();
		}

		else {
			glBindTexture(GL_TEXTURE_2D, 0); // this is garbo
		}
	}

	void GLPipelineParam::SetAsImage(
		const ITexture* texture,
		int index,
		unsigned mipmap)
	{
		if (index < 0) {
			index = m_imageCount;
		}

#ifdef IW_DEBUG
		if (index > MAX_IMAGES) {
			LOG_WARNING << "Cannot bind more than " << MAX_IMAGES << " images!";
		}
#endif

		//glUniform1i(m_location, index);
		//glActiveTexture(GL_TEXTURE0 + index);

		if (texture) {
			const GLTexture* tex = static_cast<const GLTexture*>(texture);
			glBindImageTexture(index, tex->Id(), mipmap, GL_TRUE, 0, GL_READ_WRITE, TRANSLATE(tex->Format(), tex->FormatType()));
		}

		else {
			//glBindImageTexture(index, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8); // ???
		}
	}
}
}
