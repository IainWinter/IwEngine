#include "iw/graphics/Texture.h"

namespace IW {
	Texture::Texture(
		int width,
		int height,
		TextureFormat format,
		unsigned char* colors)
		: Width(width)
		, Height(height)
		, Format(format)
		, Colors(colors)
		, Handle(nullptr)
	{
		switch (format) {
			case ALPHA:   Channels = 1;
			case RGB:     Channels = 3;
			case RGBA:    Channels = 4;
			case DEPTH:   Channels = 1;
			case STENCIL: Channels = 1;
		}
	}

	void Texture::Initialize(
		const iwu::ref<IDevice>& device)
	{
		Handle = device->CreateTexture(Width, Height, Format, Colors);
	}
}
