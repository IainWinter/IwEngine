#include "iw/renderer/Platform/OpenGL/GLTexture.h"
#include "iw/renderer/Platform/OpenGL/GLTranslator.h"
#include "gl/glew.h"
#include "iw/log/logger.h"

char const* GetErrorString(
	GLenum const err) noexcept
{
	switch (err) {
		case GL_NO_ERROR:                      return "GL_NO_ERROR";
		case GL_INVALID_ENUM:                  return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE:                 return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION:             return "GL_INVALID_OPERATION";
		case GL_STACK_OVERFLOW:                return "GL_STACK_OVERFLOW";
		case GL_STACK_UNDERFLOW:               return "GL_STACK_UNDERFLOW";
		case GL_OUT_OF_MEMORY:                 return "GL_OUT_OF_MEMORY";
		case GL_TABLE_TOO_LARGE:               return "GL_TABLE_TOO_LARGE";
		case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
		default:                               return "unknown error";
	}
}

bool MessageCallback(
	const char* stmt, 
	const char* fname, 
	int line)
{
	GLenum err = glGetError();
	bool hasError = err != GL_NO_ERROR;
	if (hasError) {
		LOG_ERROR << "GL ERROR: " << GetErrorString(err) << " in file " << fname << "[" << line << "] " << stmt;
	}

	return hasError;
}

namespace iw {
namespace RenderAPI {
	GLTexture::GLTexture(
		int width,
		int height,
		TextureType type,
		TextureFormat format,
		TextureFormatType formatType,
		TextureWrap wrap,
		const void* data)
		: m_width(width)
		, m_height(height)
		, m_format(format)
		, m_type(type)
		, m_wrapX(wrap)
		, m_wrapY(wrap)
		, m_data(data)
	{
		switch (formatType) {
			case ALPHA:   m_channels = 1; break;
			case RG:      m_channels = 2; break;
			case RGB:     m_channels = 3; break;
			case RGBA:    m_channels = 4; break;
			case DEPTH:   m_channels = 1; break;
			case STENCIL: m_channels = 1; break;
		}

		gl_type = TRANSLATE(type);
		gl_format = TRANSLATE(format, formatType);
		gl_formatType = TRANSLATE(formatType);
		gl_wrapX = gl_wrapY = TRANSLATE(wrap);

		bool errout = false;
		if (gl_formatType == GL_INVALID_VALUE) {
			LOG_ERROR << "Invalid texture format type " << formatType;
			errout = true;
		}

		if (gl_format == GL_INVALID_VALUE) {
			LOG_ERROR << "Invalid texture pixel format " << format << " channels";
			errout = true;
		}

		if (errout) {
			return;
		}

		glGenTextures(1, &gl_id);
		Bind();

		// need to add option for mip map levels

		//if (data == nullptr) {
		//	switch (type) {
		//		case TEX_2D: {
		//			glTexStorage2D(gl_type, 1, gl_format, m_width, m_height);
		//			break;
		//		}
		//		case TEX_CUBE: {
		//			for (unsigned i = 0; i < 6; i++) {
		//				glTexStorage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 1, gl_format, m_width, m_height);
		//			}
//
		//			break;
		//		}
		//	}
		//}

		//else {
			switch (type) {
				case TEX_2D: {
					glTexImage2D(gl_type, 0, gl_format, m_width, m_height, 0, TRANSLATE(format), gl_formatType, m_data);
					break;
				}
				case TEX_CUBE: {
					for (unsigned i = 0; i < 6; i++) {
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, gl_format, m_width, m_height, 0, TRANSLATE(format), gl_formatType, m_data);
					}

					break;
				}
			}
		//}

		glGenerateMipmap(gl_type);

		// Need to pass options for these
		glTexParameteri(gl_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(gl_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(gl_type, GL_TEXTURE_WRAP_S, gl_wrapX);
		glTexParameteri(gl_type, GL_TEXTURE_WRAP_T, gl_wrapY);

		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(gl_type, GL_TEXTURE_BORDER_COLOR, borderColor);
	}

	GLTexture::~GLTexture() {
		glDeleteTextures(1, &gl_id);
	}

	GLTexture* GLTexture::CreateSubTexture(
		int xOffset, 
		int yOffset, 
		int width, 
		int height,
		int mipmap) const
	{
		if (m_type != TEX_2D) { // temp
			LOG_WARNING << "idk what to do for a sub texture of a cube...";
			return nullptr;
		}

		GLTexture* sub = new GLTexture(width, height, m_type, m_format, m_formatType, m_wrapX/*, m_wrapY*/);
		if (m_data) {
			glTextureSubImage2D(sub->Id(), mipmap, xOffset, yOffset, width, height, TRANSLATE(m_format), gl_formatType, m_data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		return sub;
	}

	void GLTexture::Bind() const {
		/**/glBindTexture(gl_type, gl_id)/*)*/;
	}

	void GLTexture::Unbind() const {
		glBindTexture(gl_type, 0);
	}

	unsigned GLTexture::Id() const {
		return gl_id;
	}

	int GLTexture::Width() const {
		return m_width;
	}

	int GLTexture::Height() const {
		return m_height;
	}

	TextureType GLTexture::Type() const {
		return m_type;
	}

	TextureFormat GLTexture::Format() const {
		return m_format;
	}
	
	TextureFormatType GLTexture::FormatType() const {
		return m_formatType;
	}

	TextureWrap GLTexture::WrapX() const {
		return m_wrapX;
	}

	TextureWrap GLTexture::WrapY() const {
		return m_wrapY;
	}
}
}
