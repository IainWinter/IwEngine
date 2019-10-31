#pragma once

#include "IwGraphics.h"
#include "Color.h"

namespace IW {
inline namespace Graphics {
	struct IWGRAPHICS_API Texture {
		unsigned char* Colors;

		int Width;
		int Height;
		int Channels;

		Texture(
			unsigned char* colors,
			int width,
			int height,
			int channels);
	};
}
}
