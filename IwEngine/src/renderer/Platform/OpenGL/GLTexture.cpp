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
		m_gltype = GLTranslator::Instance().Translate(type);
		m_glformat = GLTranslator::Instance().Translate(format, type);
		m_glwrapX = m_glwrapY = GLTranslator::Instance().Translate(wrap);

		bool errout = false;
		if (m_gltype == GL_INVALID_VALUE) {
			LOG_ERROR << "Invalid texture format type " << type;
			errout = true;
		}

		if (m_glformat == GL_INVALID_VALUE) {
			LOG_ERROR << "Invalid texture pixel format " << format << " channels";
			errout = true;
		}

		if (errout) {
			return;
		}

		glGenTextures(1, &m_renderId);
		Bind();

		if (data == nullptr) {
			glTexStorage2D(GL_TEXTURE_2D, 1, m_glformat, m_width, m_height);
		}

		else {
			glTexImage2D(GL_TEXTURE_2D, 0, m_glformat, m_width, m_height, 0, m_glformat, m_gltype, m_data);
		}

		glGenerateMipmap(GL_TEXTURE_2D);

		// Need to pass options for these
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_glwrapX);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_glwrapY);

		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	}

	GLTexture::~GLTexture() {
		glDeleteTextures(1, &m_renderId);
	}

	GLTexture* GLTexture::CreateSubTexture(
		int xOffset, 
		int yOffset, 
		int width, 
		int height,
		int mipmap) const
	{
		GLTexture* sub = new GLTexture(width, height, m_format, m_type, m_wrapX/*, m_wrapY*/);
		glTextureSubImage2D(sub->m_renderId, mipmap, xOffset, yOffset, width, height, m_glformat, m_gltype, m_data);
		glGenerateMipmap(GL_TEXTURE_2D);

		return sub;
	}

	void GLTexture::Bind() const {
		glBindTexture(GL_TEXTURE_2D, m_renderId);
	}

	void GLTexture::Unbind() const {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	unsigned int GLTexture::Id() const {
		return m_renderId;
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
