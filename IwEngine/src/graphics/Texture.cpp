#include "iw/graphics/Texture.h"

namespace IW {
namespace Graphics {
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
		unsigned char* colors = nullptr)
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

		if (!Colors) {
			Colors = new unsigned char[Width * Height];
		}
	}

	Texture::Texture(
		const Texture& other)
		: Width(other.Width)
		, Height(other.Height)
		, Format(other.Format)
		, Channels(other.Channels)
		, Handle(nullptr)
	{
		Colors = new unsigned char[Width * Height];
		if (other.Colors) {
			memcpy(Colors, other.Colors, Width * Height);
		}
	}

	Texture::Texture(
		Texture&& other) noexcept
		: Width(other.Width)
		, Height(other.Height)
		, Format(other.Format)
		, Channels(other.Channels)
		, Colors(other.Colors)
		, Handle(other.Handle)
	{
		other.Colors = nullptr;
		other.Handle = nullptr;
	}

	Texture::~Texture() {
		delete Handle;
		delete[] Colors;
	}

	Texture& Texture::operator=(
		const Texture& other)
	{
		Width = other.Width;
		Height = other.Height;
		Format = other.Format;
		Channels = other.Channels;
		Handle = nullptr;

		Colors = new unsigned char[Width * Height];
		if (other.Colors) {
			memcpy(Colors, other.Colors, Width * Height);
		}

		return *this;
	}

	Texture& Texture::operator=(
		Texture&& other) noexcept
	{
		Width = other.Width;
		Height = other.Height;
		Format = other.Format;
		Channels = other.Channels;
		Colors = other.Colors;
		Handle = other.Handle;

		other.Colors = nullptr;
		other.Handle = nullptr;

		return *this;
	}

	void Texture::Initialize(
		const iw::ref<IDevice>& device)
	{
		Handle = device->CreateTexture(Width, Height, Format, FormatType, Colors);
	}

	Texture Texture::GetSubTexture(
		const iw::ref<IDevice>& device, 
		int xOffset, 
		int yOffset, 
		int width, 
		int height)
	{
		Texture texture(width, height, Format, FormatType, nullptr);
		texture.Handle = device->CreateSubTexture(Handle, xOffset, yOffset, width, height);

		return texture;
	}

	iw::vector2 Texture::Dimensions() const {
		return iw::vector2(Width, Height);
	}
}
}
