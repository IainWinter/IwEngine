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
		int m_channels;

	public:
		GLTexture(
			int width,
			int height,
			int channels,
			const unsigned char* data);

		~GLTexture();

		void UpdateData(
			const unsigned char* data,
			int width,
			int height,
			int channels) const;

		void Bind() const;
		void Unbind() const;
	};
}
}
