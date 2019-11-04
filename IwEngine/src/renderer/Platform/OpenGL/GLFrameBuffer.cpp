#include "iw/renderer/Platform/OpenGL/GLFrameBuffer.h"
#include "gl/glew.h"

namespace IW {
	GLFrameBuffer::GLFrameBuffer() {
		glGenFramebuffers(1, &m_renderId);
		Bind();
	}

	GLFrameBuffer::~GLFrameBuffer() {
		glDeleteFramebuffers(1, &m_renderId);
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

		glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture->Id(), 0);
		glDrawBuffer(GL_NONE);
	}

	void GLFrameBuffer::Bind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, m_renderId);
	}

	void GLFrameBuffer::Unbind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
