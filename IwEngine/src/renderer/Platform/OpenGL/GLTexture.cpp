#include "iw/renderer/Platform/OpenGL/GLTexture.h"
#include "gl/glew.h"
#include "iw/log/logger.h"

namespace IW {
	GLTexture::GLTexture(
		int width,
		int height,
		TextureFormat format,
		TextureFormatType type,
		const unsigned char* data)
		: m_data(data)
		, m_width(width)
		, m_height(height)
		, m_format(format)
	{
		glGenTextures(1, &m_renderId);
		Bind();

		GLenum gltype  = -1;
		GLint glformat = -1;
		int channels   = -1;
		switch (format) {
			case ALPHA:   glformat = GL_RED;                channels = 1; break;
			case RG:      glformat = GL_RG;                 channels = 2; break;
			case RGB:     glformat = GL_RGB;                channels = 3; break;
			case RGBA:    glformat = GL_RGBA;               channels = 4; break;
			case DEPTH:   glformat = GL_DEPTH_COMPONENT;    channels = 1; break;
			case STENCIL: glformat = GL_STENCIL_COMPONENTS; channels = 1; break;
			default: LOG_ERROR << "Invalid texture pixel format " << format << " channels"; break;
		}

		switch (type) {
			case UBYTE: gltype = GL_UNSIGNED_BYTE; break;
			case FLOAT: gltype = GL_FLOAT;         break;
			default: LOG_ERROR << "Invalid texture pixel type " << type;  break;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, glformat, m_width, m_height, 0, glformat, gltype, m_data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Need to pass options for these
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	GLTexture::~GLTexture() {
		glDeleteTextures(1, &m_renderId);
	}

	void GLTexture::UpdateData(
		const unsigned char* data,
		int width,
		int height,
		int channels) const
	{
		//glTexSubImage2D();
	}

	void GLTexture::Bind() const {
		glBindTexture(GL_TEXTURE_2D, m_renderId);
	}

	void GLTexture::Unbind() const {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
