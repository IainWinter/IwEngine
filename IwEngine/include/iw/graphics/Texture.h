#pragma once

#include "IwGraphics.h"
#include "Color.h"
#include "iw/renderer/Device.h"
#include "iw/renderer/Texture.h"
#include "iw/util/memory/ref.h"

namespace iw {
namespace Graphics {
	struct Texture {
	protected:
		int m_width;
		int m_height;
		int m_channels;

		TextureType       m_type;
		TextureFormat     m_format;
		TextureFormatType m_formatType;
		TextureWrap       m_wrap;

		const Texture* m_parent;
		int m_xOffset;
		int m_yOffset;

		unsigned char* m_colors;
		ITexture* m_handle;

	public:
		IWGRAPHICS_API
		Texture();

		IWGRAPHICS_API
		Texture(
			int width,
			int height,
			TextureType type,
			TextureFormat format,
			TextureFormatType formatType,
			TextureWrap wrap,
			unsigned char* colors = nullptr);

		IWGRAPHICS_API
		Texture(
			const Texture* parent,
			int xOffset,
			int yOffset,
			int width,
			int height,
			unsigned char* colors = nullptr);

		GEN_5(IWGRAPHICS_API, Texture)

		IWGRAPHICS_API
		void Initialize(
			const iw::ref<IDevice>& device);

		IWGRAPHICS_API
		Texture CreateSubTexture(
			int xOffset,
			int yOffset,
			int width,
			int height/*,
			int mipmap = 0*/) const;

		IWGRAPHICS_API
		int Width() const;

		IWGRAPHICS_API
		int Height() const;

		IWGRAPHICS_API
		int Channels() const;

		IWGRAPHICS_API
		TextureFormat Format() const;

		IWGRAPHICS_API
		TextureFormatType FormatType() const;

		IWGRAPHICS_API
		TextureWrap Wrap() const;

		IWGRAPHICS_API
		const Texture* Parent() const;

		IWGRAPHICS_API
		int XOffset() const;

		IWGRAPHICS_API
		int YOffset() const;

		IWGRAPHICS_API
		unsigned char* Colors() const;

		IWGRAPHICS_API
		ITexture* Handle() const;

		IWGRAPHICS_API
		bool IsSubTexture() const;

		IWGRAPHICS_API
		iw::vector2 Dimensions() const;
	};
}

	using namespace Graphics;
}
