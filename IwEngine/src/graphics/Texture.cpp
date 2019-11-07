#include "iw/graphics/Texture.h"

namespace IW {
	Texture::Texture()
		: Width(0)
		, Height(0)
		, Format(ALPHA)
		, FormatType(UBYTE)
		, Colors(nullptr)
		, Handle(nullptr)
	{}

	Texture::Texture(
		int width,
		int height,
		TextureFormat format,
		TextureFormatType type,
		unsigned char* colors)
		: Width(width)
		, Height(height)
		, Format(format)
		, FormatType(type)
		, Colors(colors)
		, Handle(nullptr)
	{
		switch (format) {
			case ALPHA:   Channels = 1; break;
			case RG:      Channels = 2; break;
			case RGB:     Channels = 3; break;
			case RGBA:    Channels = 4; break;
			case DEPTH:   Channels = 1; break;
			case STENCIL: Channels = 1; break;
		}
	}

	void Texture::Initialize(
		const iwu::ref<IDevice>& device)
	{
		Handle = device->CreateTexture(Width, Height, Format, FormatType, Colors);
	}
}
