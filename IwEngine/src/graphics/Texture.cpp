#include "iw/graphics/Texture.h"

namespace IW {
	Texture::Texture(
		int width,
		int height,
		int channels,
		unsigned char* colors)
		: Width(width)
		, Height(height)
		, Channels(channels)
		, Colors(colors)
		, Handle(nullptr)
	{}

	void Texture::Initialize(
		const iwu::ref<IDevice>& device)
	{
		Handle = device->CreateTexture(Width, Height, Channels, Colors);
	}
}
