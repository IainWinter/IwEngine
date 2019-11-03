#include "iw/renderer/Platform/OpenGL/GLTexture.h"
#include "gl/glew.h"
#include "iw/log/logger.h"

namespace IW {
	GLTexture::GLTexture(
		int width,
		int height,
		TextureFormat format,
		const unsigned char* data)
		: m_data(data)
		, m_width(width)
		, m_height(height)
		, m_format(format)
	{
		glGenTextures(1, &m_renderId);
		Bind();

		// Need to pass options for these
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		GLint glformat = 0;
		GLenum gltype  = GL_UNSIGNED_BYTE;
		switch (format) {
			case ALPHA:   glformat = GL_ALPHA; break;
			case RGB:     glformat = GL_RGB;   break;
			case RGBA:    glformat = GL_RGBA;  break;
			case DEPTH:   glformat = GL_DEPTH_COMPONENT;    gltype = GL_FLOAT; break;
			case STENCIL: glformat = GL_STENCIL_COMPONENTS; gltype = GL_FLOAT; break;
			default: LOG_ERROR << "Invalid format " << format << " channels"; break;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, glformat, width, height, 0, glformat, gltype, data);
		glGenerateMipmap(GL_TEXTURE_2D);
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
