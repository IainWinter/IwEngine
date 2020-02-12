#include "iw/renderer/Platform/OpenGL/GLFrameBuffer.h"
#include "gl/glew.h"

namespace iw {
namespace RenderAPI {
	GLFrameBuffer::GLFrameBuffer() 
		: m_textureCount(0)
	{
		glGenFramebuffers(1, &gl_id);
		Bind();
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

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture->Id(), 0);
		//glDrawBuffer(GL_NONE);
		//glReadBuffer(GL_NONE);
	}

	void GLFrameBuffer::Bind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, gl_id);
	}

	void GLFrameBuffer::Unbind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
}
