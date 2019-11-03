#pragma once

#include "IwGraphics.h"
#include "Color.h"
#include "iw/renderer/Device.h"
#include "iw/renderer/Texture.h"
#include "iw/util/memory/smart_pointers.h"

namespace IW {
inline namespace Graphics {
	struct IWGRAPHICS_API Texture {
		unsigned char* Colors;

		int Width;
		int Height;
		int Channels;
		TextureFormat Format;

		ITexture* Handle;

		Texture(
			int width,
			int height,
			TextureFormat format,
			unsigned char* colors);

		void Initialize(
			const iwu::ref<IDevice>& device);
	};
}
}
