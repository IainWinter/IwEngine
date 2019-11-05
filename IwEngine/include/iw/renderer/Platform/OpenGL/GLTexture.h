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
		const unsigned char* m_data;

		int m_width;
		int m_height;
		TextureFormat m_format;

	public:
		GLTexture(
			int width,
			int height,
			TextureFormat format,
			TextureFormatType type,
			const unsigned char* data);

		~GLTexture();

		void UpdateData(
			const unsigned char* data,
			int width,
			int height,
			int channels) const;

		inline unsigned int Id() const {
			return m_renderId;
		}

		inline const unsigned char* Pixels() const {
			return m_data;
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
