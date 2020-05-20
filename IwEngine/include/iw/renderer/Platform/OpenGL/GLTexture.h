#pragma once

#include "GLVertexBuffer.h"
#include "iw/renderer/Texture.h"

namespace iw {
namespace RenderAPI {
	class GLTexture
		: public ITexture
	{
	private:
		unsigned gl_id;
		int gl_type;
		int gl_iformat;
		int gl_format;
		int gl_formatType;
		int gl_wrap;
		int gl_filter;
		int gl_mipmapFilter;

		int m_width;
		int m_height;
		int m_channels;
		//int m_depth;

		TextureType         m_type;
		TextureFormat       m_format;
		TextureFormatType   m_formatType;
		TextureWrap         m_wrap;
		TextureFilter       m_filter;
		TextureMipmapFilter m_mipmapFilter;

		const void* m_data;

	public:
		IWRENDERER_API
		GLTexture(
			int width,
			int height,
			TextureType type,
			TextureFormat format,
			TextureFormatType formatType,
			TextureWrap wrap,
			TextureFilter filter,
			TextureMipmapFilter mipmapFilter,
			const void* data = nullptr);

		//IWRENDERER_API
		//GLTexture(
		//	int width,
		//	int height,
		//	TextureFormat format,
		//	TextureFormatType type,
		//	TextureWrap wrapX,
		//	TextureWrap wrapY,
		//	const void* data = nullptr);

		//IWRENDERER_API
		//GLTexture(
		//	int width,
		//	int height,
		//	int depth,
		//	TextureFormat format,
		//	TextureFormatType type,
		//	TextureWrap wrapX,
		//	TextureWrap wrapY,
		//	TextureWrap wrapZ,
		//	const void* data = nullptr);

		IWRENDERER_API
		~GLTexture();

		IWRENDERER_API
		GLTexture* CreateSubTexture(
			int xOffset,
			int yOffset,
			int width,
			int height,
			int mipmap = 0) const;

		IWRENDERER_API
		void Bind() const;

		IWRENDERER_API
		void Unbind() const;

		IWRENDERER_API
		void GenerateMipMaps() const override;

		IWRENDERER_API 
		void Clear(
			const void* color) const override;

		// settings

		IWRENDERER_API
		void SetBorderColor(
			const void* color) const override;

		IWRENDERER_API void SetFilter      (TextureFilter       filter)       override;
		IWRENDERER_API void SetMipmapFilter(TextureMipmapFilter mipmapFilter) override;

		IWRENDERER_API
		unsigned Id() const;

		IWRENDERER_API
		int Width() const;

		IWRENDERER_API
		int Height() const;

		IWRENDERER_API
		TextureType Type() const;

		IWRENDERER_API
		TextureFormat Format() const;

		IWRENDERER_API
		TextureFormatType FormatType() const;

		IWRENDERER_API
		TextureWrap Wrap() const;

		IWRENDERER_API
		TextureFilter Filter() const;

		IWRENDERER_API
		TextureMipmapFilter MipmapFilter() const;
	};
}

	using namespace RenderAPI;
}
