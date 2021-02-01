#include "iw/renderer/Platform/OpenGL/GLTexture.h"
#include "iw/renderer/Platform/OpenGL/GLTranslator.h"
#include "iw/renderer/Platform/OpenGL/GLErrorCatch.h"
#include "iw/log/logger.h"
#include "gl/glew.h"

namespace iw {
namespace RenderAPI {
	GLTexture::GLTexture(
		int width,
		int height,
		TextureType type,
		TextureFormat format,
		TextureFormatType formatType,
		TextureWrap wrap,
		TextureFilter filter,
		TextureMipmapFilter mipmapFilter,
		void* data)
		: m_width       (width)
		, m_height      (height)
		, m_format      (format)
		, m_type        (type)
		, m_formatType  (formatType)
		, m_wrap        (wrap)
		, m_filter      (filter)
		, m_mipmapFilter(mipmapFilter)
		, m_data        (data)
	{
		switch (format) {
			case ALPHA:   m_channels = 1; break;
			case RG:      m_channels = 2; break;
			case RGB:     m_channels = 3; break;
			case RGBA:    m_channels = 4; break;
			case DEPTH:   m_channels = 1; break;
			case STENCIL: m_channels = 1; break;
		}

		gl_type         = TRANSLATE(type);
		gl_iformat      = TRANSLATE(format, formatType);
		gl_format       = TRANSLATE(format);
		gl_formatType   = TRANSLATE(formatType);
		gl_wrap         = TRANSLATE(wrap);
		gl_filter       = TRANSLATE(filter);
		gl_mipmapFilter = TRANSLATE(mipmapFilter);

		if (gl_type == GL_INVALID_VALUE) {
			LOG_ERROR << "Invalid texture type " << type;
			return;
		}
		
		if (gl_iformat == GL_INVALID_VALUE) {
			LOG_ERROR << "Invalid internal texture pixel format " << format << " " << formatType;
			return;
		}

		if (gl_format == GL_INVALID_VALUE) {
			LOG_ERROR << "Invalid texture pixel format " << format;
			return;
		}

		if (gl_formatType == GL_INVALID_VALUE) {
			LOG_ERROR << "Invalid texture format type " << formatType;
			return;
		}

		if (gl_wrap == GL_INVALID_VALUE) {
			LOG_ERROR << "Invalid texture wrapping " << wrap;
			return;
		}

		if (gl_filter == GL_INVALID_VALUE) {
			LOG_ERROR << "Invalid texture filtering " << filter;
			return;
		}

		if (gl_mipmapFilter == GL_INVALID_VALUE) {
			LOG_ERROR << "Invalid texture mipmap filtering " << filter;
			return;
		}

		GL(glGenTextures(1, &gl_id));
		Bind();

		// need to add option for mip map levels

		//if (data == nullptr) {
		//	switch (type) {
		//		case TEX_2D: {
		//			GL(glTexStorage2D(gl_type, 1, gl_iformat, m_width, m_height));
		//			break;
		//		}
		//		case TEX_3D: {
		//			GL(glTexStorage3D(gl_type, 1, gl_iformat, m_width, m_height, m_width/*should be depth*/));
		//			break;
		//		}
		//		case TEX_CUBE: {
		//			for (unsigned i = 0; i < 6; i++) {
		//				GLenum gl_face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
		//				GL(glTexStorage2D(gl_face, 1, gl_iformat, m_width, m_height));
		//			}

		//			break;
		//		}
		//	}
		//}

		//else {
			switch (type) {
				case TEX_2D: {
					GL(glTexImage2D(gl_type, 0, gl_iformat, m_width, m_height, 0, gl_format, gl_formatType, m_data));
					break;
				}
				case TEX_3D: {
					GL(glTexImage3D(gl_type, 0, gl_iformat, m_width, m_height, m_height/*should be depth*/, 0, gl_format, gl_formatType, m_data));
					break;
				}
				case TEX_CUBE: {
					for (unsigned i = 0; i < 6; i++) {
						GLenum gl_face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
						GL(glTexImage2D(gl_face, 0, gl_iformat, m_width, m_height, 0, gl_format, gl_formatType, m_data));
					}

					break;
				}
			}
		//}

		GL(glTexParameteri(gl_type, GL_TEXTURE_MAG_FILTER, gl_filter));
		GL(glTexParameteri(gl_type, GL_TEXTURE_MIN_FILTER, gl_mipmapFilter));
		GL(glTexParameteri(gl_type, GL_TEXTURE_WRAP_S, gl_wrap));
		GL(glTexParameteri(gl_type, GL_TEXTURE_WRAP_T, gl_wrap));
		GL(glTexParameteri(gl_type, GL_TEXTURE_WRAP_R, gl_wrap));

		GL(glGenerateMipmap(gl_type));

		//GLfloat color[4] = {1, 1, 1, 1}; // not always RGBA!!!
		//SetBorderColor(color); // only for shadows, but works as a default?

		Unbind();
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
		if (m_type != TEX_2D) { // temp
			LOG_WARNING << "idk what to do for a sub texture of a cube...";
			return nullptr;
		}

		GLTexture* sub = new GLTexture(width, height, m_type, m_format, m_formatType, m_wrap, m_filter, m_mipmapFilter);
		if (m_data) {
			sub->SetTextureData(this, xOffset, yOffset, width, height, mipmap);
		}

		return sub;
	}

	void GLTexture::SetTextureData(
		const GLTexture* source,
		int xOffset,
		int yOffset,
		int width,
		int height,
		int mipmap) const
	{
		if (m_data) {
			GL(glTextureSubImage2D(gl_id, mipmap, xOffset, yOffset, width, height, gl_format, gl_formatType, source->m_data));
			GenerateMipMaps();
		}
	}

	void GLTexture::Bind() const {
		GL(glBindTexture(gl_type, gl_id));
	}

	void GLTexture::Unbind() const {
		GL(glBindTexture(gl_type, 0));
	}

	void GLTexture::GenerateMipMaps() const {
		Bind();
		GL(glGenerateMipmap(gl_type));
		Unbind();
	}

	void GLTexture::Clear(
		const void* color) const
	{
		GL(glClearTexImage(gl_id, 0, gl_format, gl_formatType, color));
	}

	void GLTexture::SetBorderColor(
		const void* color) const
	{
		switch (FormatType()) {
			case FLOAT: GL(glTexParameterfv(gl_type, GL_TEXTURE_BORDER_COLOR, (float*)color)); break;
			case UBYTE: GL(glTexParameteriv(gl_type, GL_TEXTURE_BORDER_COLOR, (int*)  color)); break; // not sure if this is legal
		}
	}

	void GLTexture::SetFilter(
		TextureFilter filter)
	{
		m_filter  = filter;
		gl_filter = TRANSLATE(filter);

		GL(glTexParameteri(gl_type, GL_TEXTURE_MAG_FILTER, gl_filter));
	}

	void GLTexture::SetMipmapFilter(
		TextureMipmapFilter mipmapFilter)
	{
		m_mipmapFilter  = mipmapFilter;
		gl_mipmapFilter = TRANSLATE(mipmapFilter);

		GL(glTexParameteri(gl_type, GL_TEXTURE_MIN_FILTER, gl_mipmapFilter));
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

	TextureType GLTexture::Type() const {
		return m_type;
	}

	TextureFormat GLTexture::Format() const {
		return m_format;
	}
	
	TextureFormatType GLTexture::FormatType() const {
		return m_formatType;
	}

	TextureWrap GLTexture::Wrap() const {
		return m_wrap;
	}

	TextureFilter GLTexture::Filter() const {
		return m_filter;
	}
	
	TextureMipmapFilter GLTexture::MipmapFilter() const {
		return m_mipmapFilter;
	}

	void* GLTexture::Data() const {
		return m_data;
	}
}
}
