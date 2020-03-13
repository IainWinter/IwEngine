#include "iw/renderer/Platform/OpenGL/GLFrameBuffer.h"
#include "gl/glew.h"

namespace iw {
namespace RenderAPI {
	GLFrameBuffer::GLFrameBuffer(
		bool noColor)
		: m_textureCount(0)
		, m_noColor(noColor)
	{
		glGenFramebuffers(1, &gl_id);
		Bind();

		if (noColor) {
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}
	}

	GLFrameBuffer::~GLFrameBuffer() {
		glDeleteFramebuffers(1, &gl_id);
	}

	void GLFrameBuffer::AttachTexture(
		GLTexture* texture)
	{
		GLint attachment = 0;
		switch (texture->Format()) {
			case DEPTH:   attachment = GL_DEPTH_ATTACHMENT;   break;
			case STENCIL: attachment = GL_STENCIL_ATTACHMENT; break;
			default:      attachment = GL_COLOR_ATTACHMENT0 + m_textureCount++;
		}

		Bind();
		texture->Bind();

		if (texture->Type() == TEX_2D) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->Id(), 0);
		}

		else if (texture->Type() == TEX_CUBE) {
			glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture->Id(), 0);
		}
	}

	void GLFrameBuffer::Bind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, gl_id);
	}

	void GLFrameBuffer::Unbind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	bool GLFrameBuffer::NoColor() const {
		return m_noColor;
	}
}
}
