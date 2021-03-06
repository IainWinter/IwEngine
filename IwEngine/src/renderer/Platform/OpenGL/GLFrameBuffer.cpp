#include "iw/renderer/Platform/OpenGL/GLFrameBuffer.h"
#include "iw/renderer/Platform/OpenGL/GLErrorCatch.h"
#include "iw/renderer/Platform/OpenGL/GLTranslator.h"
#include "gl/glew.h"

namespace iw {
namespace RenderAPI {
	GLFrameBuffer::GLFrameBuffer(
		bool noColor)
		: m_textureCount(0)
		, m_noColor(noColor)
	{
		GL(glGenFramebuffers(1, &gl_id));
		Bind();

		if (noColor) {
			GL(glDrawBuffer(GL_NONE));
			GL(glReadBuffer(GL_NONE));
		}
	}

	GLFrameBuffer::~GLFrameBuffer() {
		GL(glDeleteFramebuffers(1, &gl_id));
	}

	void GLFrameBuffer::AttachTexture(
		GLTexture* texture)
	{
		GLenum attachment = 0;
		switch (texture->Format()) {
			case DEPTH:   attachment = GL_DEPTH_ATTACHMENT;   break;
			case STENCIL: attachment = GL_STENCIL_ATTACHMENT; break;
			default:      attachment = GL_COLOR_ATTACHMENT0 + m_textureCount++;
		}

		m_attachments.emplace_back(attachment, texture);

		Bind();
		texture->Bind();

		if (texture->Type() == TEX_2D) {
			GL(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->Id(), 0));
		}

		else if (texture->Type() == TEX_CUBE) {
			GL(glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture->Id(), 0));
		}

		texture->Unbind();
		Unbind();
	}

	void GLFrameBuffer::ReadPixels() {
		Bind();
		
		for (auto [attachment, texture] : m_attachments) {
			glReadBuffer(attachment);
			glReadPixels(0, 0, texture->Width(), texture->Height(),
				TRANSLATE(texture->Format()),
				TRANSLATE(texture->FormatType()),
				texture->Data());
		}

		Unbind();
	}

	void GLFrameBuffer::Bind() const {
		GL(glBindFramebuffer(GL_FRAMEBUFFER, gl_id));
	}

	void GLFrameBuffer::Unbind() const {
		GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	bool GLFrameBuffer::NoColor() const {
		return m_noColor;
	}
}
}
