#include "iw/graphics/Texture.h"

namespace iw {
namespace Graphics {
	Texture::Texture()
		: m_width(0)
		, m_height(0)
		, m_channels(0)
		, m_format(ALPHA)
		, m_formatType(UBYTE)
		, m_wrap(REPEAT)
		, m_parent(nullptr)
		, m_xOffset(0)
		, m_yOffset(0)
		, m_colors(nullptr)
		, m_handle(nullptr)
	{}

	Texture::Texture(
		int width,
		int height,
		TextureFormat format,
		TextureFormatType type,
		TextureWrap wrap,
		unsigned char* colors)
		: m_width(width)
		, m_height(height)
		, m_format(format)
		, m_formatType(type)
		, m_wrap(wrap)
		, m_parent(nullptr)
		, m_xOffset(0)
		, m_yOffset(0)
		, m_colors(colors)
		, m_handle(nullptr)
	{
		switch (format) {
			case ALPHA:   m_channels = 1; break;
			case RG:      m_channels = 2; break;
			case RGB:     m_channels = 3; break;
			case RGBA:    m_channels = 4; break;
			case DEPTH:   m_channels = 1; break;
			case STENCIL: m_channels = 1; break;
		}

		//if (!m_colors) {
		//	m_colors = new unsigned char[m_width * m_height];
		//	memset(m_colors, 0, m_width * m_height);
		//}
	}

	Texture::Texture(
		const Texture* parent,
		int xOffset, 
		int yOffset, 
		int width, 
		int height, 
		unsigned char* colors)
		: m_width(width)
		, m_height(height)
		, m_channels(parent->m_channels)
		, m_format(parent->m_format)
		, m_formatType(parent->m_formatType)
		, m_wrap(parent->m_wrap)
		, m_parent(parent)
		, m_xOffset(xOffset)
		, m_yOffset(yOffset)
		, m_colors(colors)
		, m_handle(nullptr)
	{}

	Texture::Texture(
		const Texture& other)
		: m_width(other.m_width)
		, m_height(other.m_height)
		, m_channels(other.m_channels)
		, m_format(other.m_format)
		, m_formatType(other.m_formatType)
		, m_wrap(other.m_wrap)
		, m_parent(other.m_parent)
		, m_xOffset(other.m_xOffset)
		, m_yOffset(other.m_yOffset)
		, m_handle(nullptr)
	{
		m_colors = new unsigned char[m_width * m_height];
		if (other.m_colors) {
			memcpy(m_colors, other.m_colors, m_width * m_height);
		}
	}

	Texture::Texture(
		Texture&& other) noexcept
		: m_width(other.m_width)
		, m_height(other.m_height)
		, m_channels(other.m_channels)
		, m_format(other.m_format)
		, m_formatType(other.m_formatType)
		, m_wrap(other.m_wrap)
		, m_parent(other.m_parent)
		, m_xOffset(other.m_xOffset)
		, m_yOffset(other.m_yOffset)
		, m_colors(other.m_colors)
		, m_handle(other.m_handle)
	{
		other.m_colors = nullptr;
		other.m_handle = nullptr;
	}

	Texture::~Texture() {
		delete m_handle;
		delete[] m_colors;
	}

	Texture& Texture::operator=(
		const Texture& other)
	{
		m_width      = other.m_width;
		m_height     = other.m_height;
		m_format     = other.m_format;
		m_formatType = other.m_formatType;
		m_channels   = other.m_channels;
		m_wrap       = other.m_wrap;
		m_parent     = other.m_parent;
		m_xOffset    = other.m_yOffset;
		m_yOffset    = other.m_yOffset;
		m_colors     = other.m_colors;
		m_handle     = nullptr;

		m_colors = new unsigned char[m_width * m_height];
		if (other.m_colors) {
			memcpy(m_colors, other.m_colors, m_width * m_height);
		}

		return *this;
	}

	Texture& Texture::operator=(
		Texture&& other) noexcept
	{
		m_width      = other.m_width;
		m_height     = other.m_height;
		m_format     = other.m_format;
		m_formatType = other.m_formatType;
		m_channels   = other.m_channels;
		m_wrap       = other.m_wrap;
		m_parent     = other.m_parent;
		m_xOffset    = other.m_yOffset;
		m_yOffset    = other.m_yOffset;
		m_colors     = other.m_colors;
		m_handle     = other.m_handle;

		other.m_colors = nullptr;
		other.m_handle = nullptr;

		return *this;
	}

	void Texture::Initialize(
		const iw::ref<IDevice>& device)
	{
		if (IsSubTexture()) {
			m_handle = device->CreateSubTexture(m_parent->Handle(), m_xOffset, m_yOffset, m_width, m_height);
		}

		else {
			m_handle = device->CreateTexture(m_width, m_height, m_format, m_formatType, m_wrap, m_colors);
		}
	}

	Texture Texture::CreateSubTexture(
		int xOffset, 
		int yOffset, 
		int width, 
		int height/*,
		int mipmap = 0*/) const
	{
		return Texture(this, xOffset, yOffset, width, height/*, minmap*/);
	}

	int Texture::Width() const {
		return m_width;
	}

	int Texture::Height() const {
		return m_height;
	}

	int Texture::Channels() const {
		return m_channels;
	}

	TextureFormat Texture::Format() const {
		return m_format;
	}

	TextureFormatType Texture::FormatType() const {
		return m_formatType;
	}

	const Texture* Texture::Parent() const {
		return m_parent;
	}

	int Texture::XOffset() const {
		return m_xOffset;
	}

	int Texture::YOffset() const {
		return m_yOffset;
	}

	unsigned char* Texture::Colors() const {
		return m_colors;
	}

	ITexture* Texture::Handle() const {
		return m_handle;
	}

	bool Texture::IsSubTexture() const {
		return !!m_parent;
	}

	iw::vector2 Texture::Dimensions() const {
		return iw::vector2(m_width, m_height);
	}
}
}
