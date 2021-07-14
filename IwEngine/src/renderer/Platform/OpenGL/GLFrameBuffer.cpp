#include "iw/renderer/Platform/OpenGL/GLFrameBuffer.h"
#include "iw/renderer/Platform/OpenGL/GLErrorCatch.h"
#include "iw/renderer/Platform/OpenGL/GLTranslator.h"
#include "iw/log/logger.h"
#include "gl/glew.h"

namespace iw {
namespace RenderAPI {
	GLFrameBuffer::GLFrameBuffer()
		: m_textureCount(0)
	{
		GL(glGenFramebuffers(1, &gl_id));
		Unbind();
	}

	GLFrameBuffer::~GLFrameBuffer() {
		GL(glDeleteFramebuffers(1, &gl_id));
	}

	bool GLFrameBuffer::AttachTexture(
		ITexture* texture_)
	{
		bool isColor = false;

		GLTexture* texture = dynamic_cast<GLTexture*>(texture_);

		GLenum attachment = 0;
		switch (texture->Format()) {
			case DEPTH:   attachment = GL_DEPTH_ATTACHMENT;   break;
			case STENCIL: attachment = GL_STENCIL_ATTACHMENT; break;
			default: {
				attachment = GL_COLOR_ATTACHMENT0 + m_textureCount++;
				isColor = true;
			}
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

		return isColor;
	}

	void GLFrameBuffer::SetDrawable(
		const std::vector<unsigned>& colorAttachments)
	{
		Bind();

		// Should validate 'colorAttachments'

		std::vector<GLenum> glAttachments;
		for (unsigned i : colorAttachments) 
		{
			glAttachments.push_back(GL_COLOR_ATTACHMENT0 + i);
		}

		GL(glDrawBuffers(glAttachments.size(), glAttachments.data()));

		Unbind();
	}

	void GLFrameBuffer::ReadPixels() {
		Bind();
		
		for (auto [attachment, texture] : m_attachments)
		{
			if (!texture->Data()) continue;

			glReadBuffer(attachment);
			glReadPixels(0, 0, texture->Width(), texture->Height(),
				TRANSLATE(texture->Format()), TRANSLATE(texture->FormatType()),
				texture->Data());
		}

		glReadBuffer(GL_NONE);

		Unbind();
	}

	void GLFrameBuffer::WritePixels() {
		for (auto [attachment, texture] : m_attachments) {
			texture->SetTextureData(texture, 0, 0, texture->Width(), texture->Height(), 0);
		}
	}

	void GLFrameBuffer::Bind() const {
		GL(glBindFramebuffer(GL_FRAMEBUFFER, gl_id));
	}

	void GLFrameBuffer::Unbind() const {
		GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}
}
}
