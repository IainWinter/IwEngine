#include "iw/renderer/Platform/OpenGL/GLFrameBuffer.h"
#include "gl/glew.h"

namespace IW {
	GLFrameBuffer::GLFrameBuffer(
		ITexture* texture)
	{
		glGenFramebuffers(1, &m_renderId);

		Bind();

		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);

	}

	GLFrameBuffer::~GLFrameBuffer() {
		glDeleteFramebuffers(1, &m_renderId);
	}

	void GLFrameBuffer::Bind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, m_renderId);
	}

	void GLFrameBuffer::Unbind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
