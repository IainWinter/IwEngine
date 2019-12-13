#pragma once

#include "IwGraphics.h"
#include "Color.h"
#include "iw/renderer/Device.h"
#include "iw/renderer/Texture.h"
#include "iw/util/memory/smart_pointers.h"

namespace IW {
namespace Graphics {
	struct IWGRAPHICS_API Texture {
		unsigned char* Colors;

		int Width;
		int Height;
		int Channels;
		TextureFormat Format;
		TextureFormatType FormatType;

		ITexture* Handle;

		Texture();

		Texture(
			int width,
			int height,
			TextureFormat format,
			TextureFormatType type,
			unsigned char* colors);

		void Initialize(
			const iw::ref<IDevice>& device);
	};
}

	using namespace Graphics;
}
