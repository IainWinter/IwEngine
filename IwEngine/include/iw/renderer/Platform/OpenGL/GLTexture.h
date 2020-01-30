#pragma once

#include "GLVertexBuffer.h"
#include "iw/renderer/Texture.h"

namespace IW {
namespace RenderAPI {
	class GLTexture
		: public ITexture
	{
	private:
		unsigned int gl_id;
		int gl_format;
		int gl_type;
		int gl_wrapX;
		int gl_wrapY;

		const void* m_data;

		int m_width;
		int m_height;
		//int m_depth;

		TextureFormatType m_type;
		TextureFormat     m_format;
		TextureWrap       m_wrapX;
		TextureWrap       m_wrapY;
		//TextureWrap       m_wrapZ;
		//int m_glwrapZ;

	public:
		IWRENDERER_API
		GLTexture(
			int width,
			int height,
			TextureFormat format,
			TextureFormatType type,
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
		unsigned int Id() const;

		IWRENDERER_API
		int Width() const;

		IWRENDERER_API
		int Height() const;

		IWRENDERER_API
		TextureFormat Format() const;

		IWRENDERER_API
		TextureFormatType Type() const;
	};
}

	using namespace RenderAPI;
}
