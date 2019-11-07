#pragma once

#include "GLVertexBuffer.h"
#include "iw/renderer/Texture.h"

namespace IW {
inline namespace RenderAPI {
	class IWRENDERER_API GLTexture
		: public ITexture
	{
	private:
		unsigned int m_renderId;
		const void* m_data;

		int m_width;
		int m_height;
		TextureFormat m_format;

	public:
		GLTexture(
			int width,
			int height,
			TextureFormat format,
			TextureFormatType type,
			const void* data);

		~GLTexture();

		inline unsigned int Id() const {
			return m_renderId;
		}

		inline int Width() const {
			return m_width;
		}

		inline int Height() const {
			return m_height;
		}

		inline TextureFormat Format() const {
			return m_format;
		}

		void Bind() const;
		void Unbind() const;
	};
}
}
