#include "iw/graphics/Texture.h"

namespace IW {
	Texture::Texture(
		unsigned char* colors,
		int width,
		int height,
		int channels)
		: Colors(colors)
		, Width(width)
		, Height(height)
		, Channels(channels)
	{}
}
