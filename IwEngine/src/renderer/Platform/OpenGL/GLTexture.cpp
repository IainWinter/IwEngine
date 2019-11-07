#include "iw/renderer/Platform/OpenGL/GLTexture.h"
#include "gl/glew.h"
#include "iw/log/logger.h"

namespace IW {
	GLTexture::GLTexture(
		int width,
		int height,
		TextureFormat format,
		TextureFormatType type,
		const void* data)
		: m_data(data)
		, m_width(width)
		, m_height(height)
		, m_format(format)
	{
		GLenum gltype = -1;
		GLint glformat = -1;
		switch (format) {
			case ALPHA: {
				switch (type) {
					case UBYTE: glformat = GL_RED;  break;
					case FLOAT: glformat = GL_R32F; break;
				}

				break;
			}

			case RG: {
				switch (type) {
					case UBYTE: glformat = GL_RG;    break;
					case FLOAT: glformat = GL_RG32F; break;
				}

				break;
			}

			case RGB: {
				switch (type) {
					case UBYTE: glformat = GL_RGB;    break;
					case FLOAT: glformat = GL_RGB32F; break;
				}

				break;
			}

			case RGBA: {
				switch (type) {
					case UBYTE: glformat = GL_RGBA;    break;
					case FLOAT: glformat = GL_RGBA32F; break;
				}

				break;
			}

			case DEPTH: {
				switch (type) {
					case UBYTE: glformat = GL_DEPTH_COMPONENT;    break;
					case FLOAT: glformat = GL_DEPTH_COMPONENT32F; break;
				}

				break;
			}

			case STENCIL: {
				glformat = GL_STENCIL_COMPONENTS;
				break;
			}

			default: {
				LOG_ERROR << "Invalid texture pixel format " << format << " channels";
				break;
			}
		}

		switch (type) {
			case UBYTE: gltype = GL_UNSIGNED_BYTE; break;
			case FLOAT: gltype = GL_FLOAT;         break;
			default: LOG_ERROR << "Invalid texture pixel type " << type;  break;
		}

		if (glformat == -1 || gltype == -1) {
			return;
		}

		glGenTextures(1, &m_renderId);
		Bind();

		if (data == nullptr) {
			glTexStorage2D(GL_TEXTURE_2D, 1, glformat, m_width, m_height);
		}

		else {
			glTexImage2D(GL_TEXTURE_2D, 0, glformat, m_width, m_height, 0, glformat, gltype, m_data);
		}

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

	void GLTexture::Bind() const {
		glBindTexture(GL_TEXTURE_2D, m_renderId);
	}

	void GLTexture::Unbind() const {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
