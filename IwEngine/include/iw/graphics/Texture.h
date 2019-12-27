#pragma once

#include "IwGraphics.h"
#include "Color.h"
#include "iw/renderer/Device.h"
#include "iw/renderer/Texture.h"
#include "iw/util/memory/smart_pointers.h"

namespace IW {
namespace Graphics {
	struct Texture {
		unsigned char* Colors;

		int Width;
		int Height;
		int Channels;
		TextureFormat Format;
		TextureFormatType FormatType;

		ITexture* Handle;

		IWGRAPHICS_API
		Texture();

		IWGRAPHICS_API
		Texture(
			int width,
			int height,
			TextureFormat format,
			TextureFormatType type,
			unsigned char* colors);

		GEN_5(IWGRAPHICS_API, Texture)

		IWGRAPHICS_API
		void Initialize(
			const iw::ref<IDevice>& device);

		IWGRAPHICS_API
		Texture GetSubTexture(
			const iw::ref<IDevice>& device,
			int xOffset,
			int yOffset,
			int width,
			int height);

		IWGRAPHICS_API
		iw::vector2 Dimensions() const;
	};
}

	using namespace Graphics;
}
