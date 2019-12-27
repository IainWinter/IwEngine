#pragma once

#include "GLVertexBuffer.h"
#include "iw/renderer/Texture.h"

namespace IW {
namespace RenderAPI {
	class GLTexture
		: public ITexture
	{
	private:
		unsigned int m_renderId;
		const void* m_data;

		int m_width;
		int m_height;
		TextureFormat m_format;
		TextureFormatType m_type;
		int m_glformat;
		int m_gltype;

	public:
		IWRENDERER_API
		GLTexture(
			int width,
			int height,
			TextureFormat format,
			TextureFormatType type,
			const void* data);

		IWRENDERER_API
		~GLTexture();

		IWRENDERER_API
		GLTexture* CreateSubTexture(
			int xOffset,
			int yOffset,
			int width,
			int height,
			int minmap = 0) const;

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
