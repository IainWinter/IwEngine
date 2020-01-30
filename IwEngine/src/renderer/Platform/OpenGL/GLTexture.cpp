#include "iw/renderer/Platform/OpenGL/GLTexture.h"
#include "iw/renderer/Platform/OpenGL/GLTranslator.h"
#include "gl/glew.h"
#include "iw/log/logger.h"

namespace IW {
	GLTexture::GLTexture(
		int width,
		int height,
		TextureFormat format,
		TextureFormatType type,
		TextureWrap wrap,
		const void* data)
		: m_data(data)
		, m_width(width)
		, m_height(height)
		, m_format(format)
		, m_type(type)
		, m_wrapX(wrap)
		, m_wrapY(wrap)
	{
		gl_type = GLTranslator::Instance().Translate(type);
		gl_format = GLTranslator::Instance().Translate(format, type);
		gl_wrapX = gl_wrapY = GLTranslator::Instance().Translate(wrap);

		bool errout = false;
		if (gl_type == GL_INVALID_VALUE) {
			LOG_ERROR << "Invalid texture format type " << type;
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

		if (data == nullptr) {
			glTexStorage2D(GL_TEXTURE_2D, 1, gl_format, m_width, m_height);
		}

		else {
			glTexImage2D(GL_TEXTURE_2D, 0, gl_format, m_width, m_height, 0, gl_format, gl_type, m_data);
		}

		glGenerateMipmap(GL_TEXTURE_2D);

		// Need to pass options for these
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gl_wrapX);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl_wrapY);

		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
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
		GLTexture* sub = new GLTexture(width, height, m_format, m_type, m_wrapX/*, m_wrapY*/);
		glTextureSubImage2D(sub->gl_id, mipmap, xOffset, yOffset, width, height, gl_format, gl_type, m_data);
		glGenerateMipmap(GL_TEXTURE_2D);

		return sub;
	}

	void GLTexture::Bind() const {
		glBindTexture(GL_TEXTURE_2D, gl_id);
	}

	void GLTexture::Unbind() const {
		glBindTexture(GL_TEXTURE_2D, 0);
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

	TextureFormat GLTexture::Format() const {
		return m_format;
	}
	
	TextureFormatType GLTexture::Type() const {
		return m_type;
	}
}
