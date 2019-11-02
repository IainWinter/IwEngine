#include "iw/renderer/Platform/OpenGL/GLTexture.h"
#include "gl/glew.h"
#include "iw/log/logger.h"

namespace IW {
	GLTexture::GLTexture(
		int width,
		int height,
		int channels,
		const unsigned char* data)
		: m_data(data)
		, m_width(width)
		, m_height(height)
		, m_channels(channels)
	{
		glGenTextures(1, &m_renderId);
		glBindTexture(GL_TEXTURE_2D, m_renderId);

		// Need to pass options for these
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		GLint format = 0;
		GLint internalformat = 0;
		switch (channels) {
			case 1: internalformat = GL_R8; format = GL_RED; break;
			case 3: internalformat = format = GL_RGB;  break;
			case 4: internalformat = format = GL_RGBA; break;
			default: 
				LOG_ERROR << "No format for " 
				<< channels 
				<< " channels"; break;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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
