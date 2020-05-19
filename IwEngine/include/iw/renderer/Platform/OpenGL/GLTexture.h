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
		int gl_format;
		int gl_formatType;
		int gl_wrapX;
		int gl_wrapY;
		int gl_wrapZ;

		int m_width;
		int m_height;
		int m_channels;
		//int m_depth;

		TextureType       m_type;
		TextureFormat     m_format;
		TextureFormatType m_formatType;
		TextureWrap       m_wrapX;
		TextureWrap       m_wrapY;
		TextureWrap       m_wrapZ;

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
			float r,
			float g,
			float b,
			float a = 1.0f) const override;

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
		TextureWrap WrapX() const;

		IWRENDERER_API
		TextureWrap WrapY() const;
	};
}

	using namespace RenderAPI;
}
