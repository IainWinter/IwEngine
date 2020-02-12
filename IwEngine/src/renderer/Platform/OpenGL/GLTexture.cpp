#include "iw/renderer/Platform/OpenGL/GLTexture.h"
#include "iw/renderer/Platform/OpenGL/GLTranslator.h"
#include "gl/glew.h"
#include "iw/log/logger.h"

char const* GetErrorString(
	GLenum const err) noexcept
{
	switch (err) {
		case GL_NO_ERROR:                      return "GL_NO_ERROR";
		case GL_INVALID_ENUM:                  return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE:                 return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION:             return "GL_INVALID_OPERATION";
		case GL_STACK_OVERFLOW:                return "GL_STACK_OVERFLOW";
		case GL_STACK_UNDERFLOW:               return "GL_STACK_UNDERFLOW";
		case GL_OUT_OF_MEMORY:                 return "GL_OUT_OF_MEMORY";
		case GL_TABLE_TOO_LARGE:               return "GL_TABLE_TOO_LARGE";
		case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
		default:                               return "unknown error";
	}
}

bool MessageCallback(
	const char* stmt, 
	const char* fname, 
	int line)
{
	GLenum err = glGetError();
	bool hasError = err != GL_NO_ERROR;
	if (hasError) {
		LOG_ERROR << "GL ERROR: " << GetErrorString(err) << " in file " << fname << "[" << line << "] " << stmt;
	}

	return hasError;
}

#ifdef IW_DEBUG
	#define GL(stmt) stmt; while(MessageCallback(#stmt, __FILE__, __LINE__)) {}
#else
	#define GL(stmt) stmt
#endif

namespace iw {
namespace RenderAPI {
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
		gl_type = TRANSLATE(type);
		gl_format = TRANSLATE(format, type);
		gl_wrapX = gl_wrapY = TRANSLATE(wrap);

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

		GL(glGenTextures(1, &gl_id));
		Bind();

		if (data == nullptr) {
			GL(glTexStorage2D(GL_TEXTURE_2D, 1, gl_format, m_width, m_height));
		}

		else {
			GL(glTexImage2D(GL_TEXTURE_2D, 0, gl_format, m_width, m_height, 0, TRANSLATE(format), gl_type, m_data));
		}

		GL(glGenerateMipmap(GL_TEXTURE_2D));

		// Need to pass options for these
		GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gl_wrapX));
		GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl_wrapY));

		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GL(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));
	}

	GLTexture::~GLTexture() {
		GL(glDeleteTextures(1, &gl_id));
	}

	GLTexture* GLTexture::CreateSubTexture(
		int xOffset, 
		int yOffset, 
		int width, 
		int height,
		int mipmap) const
	{
		GLTexture* sub = new GLTexture(width, height, m_format, m_type, m_wrapX/*, m_wrapY*/);
		if (m_data) {
			GL(glTextureSubImage2D(sub->Id(), mipmap, xOffset, yOffset, width, height, TRANSLATE(m_format), gl_type, m_data));
			GL(glGenerateMipmap(GL_TEXTURE_2D));
		}

		return sub;
	}

	void GLTexture::Bind() const {
		/*GL(*/glBindTexture(GL_TEXTURE_2D, gl_id)/*)*/; // this throws a GL_INVALID_OPERATION for some reason on the albedo maps no clue why
	}

	void GLTexture::Unbind() const {
		GL(glBindTexture(GL_TEXTURE_2D, 0));
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
}
