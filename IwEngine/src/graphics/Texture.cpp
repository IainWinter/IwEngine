#include "iw/graphics/Texture.h"

namespace iw {
namespace Graphics {
	Texture::Texture()
		: m_width        (0)
		, m_height       (0)
		, m_depth        (0)
		, m_channels     (0)
		, m_type         (TEX_2D)
		, m_format       (ALPHA)
		, m_formatType   (UBYTE)
		, m_wrap         (REPEAT)
		, m_filter       (LINEAR)
		, m_mipmapFilter (LINEAR_LINEAR)
		, m_parent       (nullptr)
		, m_xOffset      (0)
		, m_yOffset      (0)
		, m_ownsColors   (false)
		, m_colors       (nullptr)
		, m_handle       (nullptr)
	{}

	Texture::Texture(
		unsigned width,
		unsigned height,
		TextureType type,
		TextureFormat format,
		TextureFormatType formatType,
		TextureWrap wrap,
		TextureFilter filter,
		TextureMipmapFilter mipmapFilter,
		unsigned char* colors)
		: m_width       (width)
		, m_height      (height)
		, m_depth       (1)
		, m_channels    (0)
		, m_type        (type)
		, m_format      (format)
		, m_formatType  (formatType)
		, m_wrap        (wrap)
		, m_filter      (filter)
		, m_mipmapFilter(mipmapFilter)
		, m_parent      (nullptr)
		, m_xOffset     (0)
		, m_yOffset     (0)
		, m_ownsColors  (false)
		, m_colors      (colors)
		, m_handle      (nullptr)
	{
		switch (format) {
			case ALPHA:   m_channels = 1; break;
			case RG:      m_channels = 2; break;
			case RGB:     m_channels = 3; break;
			case RGBA:    m_channels = 4; break;
			case DEPTH:   m_channels = 1; break;
			case STENCIL: m_channels = 1; break;
		}
	}

	Texture::Texture(
		unsigned width,
		unsigned height,
		unsigned depth,
		TextureType type,
		TextureFormat format,
		TextureFormatType formatType,
		TextureWrap wrap,
		TextureFilter filter,
		TextureMipmapFilter mipmapFilter,
		unsigned char* colors)
		: m_width       (width)
		, m_height      (height)
		, m_depth       (depth)
		, m_channels    (0)
		, m_type        (type)
		, m_format      (format)
		, m_formatType  (formatType)
		, m_wrap        (wrap)
		, m_filter      (filter)
		, m_mipmapFilter(mipmapFilter)
		, m_parent      (nullptr)
		, m_xOffset     (0)
		, m_yOffset     (0)
		, m_colors      (colors)
		, m_ownsColors  (false)
		, m_handle      (nullptr)
	{
		switch (format) {
			case ALPHA:   m_channels = 1; break;
			case RG:      m_channels = 2; break;
			case RGB:     m_channels = 3; break;
			case RGBA:    m_channels = 4; break;
			case DEPTH:   m_channels = 1; break;
			case STENCIL: m_channels = 1; break;
		}
	}

	// cant handle 3d textures
	Texture::Texture(
		/*const*/ Texture* parent,
		int xOffset, 
		int yOffset, 
		unsigned width,
		unsigned height,
		unsigned char* colors)
		: m_width       (width)
		, m_height      (height)
		, m_depth       (0)
		, m_channels    (parent->m_channels)
		, m_type        (parent->m_type)
		, m_format      (parent->m_format)
		, m_formatType  (parent->m_formatType)
		, m_wrap        (parent->m_wrap)
		, m_filter      (parent->m_filter)
		, m_mipmapFilter(parent->m_mipmapFilter)
		, m_parent      (parent)
		, m_xOffset     (xOffset)
		, m_yOffset     (yOffset)
		, m_ownsColors  (false)
		, m_colors      (colors)
		, m_handle      (nullptr)
	{}

	Texture::Texture(
		const Texture& other)
		: m_width       (other.m_width)
		, m_height      (other.m_height)
		, m_depth       (other.m_depth)
		, m_channels    (other.m_channels)
		, m_type        (other.m_type)
		, m_format      (other.m_format)
		, m_formatType  (other.m_formatType)
		, m_wrap        (other.m_wrap)
		, m_filter      (other.m_filter)
		, m_mipmapFilter(other.m_mipmapFilter)
		, m_parent      (other.m_parent)
		, m_xOffset     (other.m_xOffset)
		, m_yOffset     (other.m_yOffset)
		, m_ownsColors  (other.m_ownsColors)
		, m_colors      (nullptr)
		, m_handle      (nullptr)
	{
		CreateColors(other.m_colors != nullptr);
		if (other.m_colors) {
			memcpy(m_colors, other.m_colors, ColorCount());
		}
	}

	Texture::Texture(
		Texture&& other) noexcept
		: m_width       (other.m_width)
		, m_height      (other.m_height)
		, m_depth       (other.m_depth)
		, m_channels    (other.m_channels)
		, m_type        (other.m_type)
		, m_format      (other.m_format)
		, m_formatType  (other.m_formatType)
		, m_wrap        (other.m_wrap)
		, m_filter      (other.m_filter)
		, m_mipmapFilter(other.m_mipmapFilter)
		, m_parent      (other.m_parent)
		, m_xOffset     (other.m_xOffset)
		, m_yOffset     (other.m_yOffset)
		, m_ownsColors  (other.m_ownsColors)
		, m_colors      (other.m_colors)
		, m_handle      (other.m_handle)
	{
		other.m_colors = nullptr;
		other.m_handle = nullptr;
	}

	Texture::~Texture() {
		delete m_handle; // somehow this can cause GL_INVALID_OP, seems to have to do with cloning texture?
		if (m_ownsColors) delete[] m_colors;
	}

	Texture& Texture::operator=(
		const Texture& other)
	{
		m_width        = other.m_width;
		m_height       = other.m_height;
		m_depth        = other.m_depth;
		m_channels     = other.m_channels;
		m_type         = other.m_type;
		m_format       = other.m_format;
		m_formatType   = other.m_formatType;
		m_wrap         = other.m_wrap;
		m_filter       = other.m_filter;
		m_mipmapFilter = other.m_mipmapFilter;
		m_parent       = other.m_parent;
		m_xOffset      = other.m_yOffset;
		m_yOffset      = other.m_yOffset;
		m_ownsColors   = other.m_ownsColors;
		m_colors       = nullptr;

		if (m_handle) {
			delete m_handle;
		}

		m_handle = nullptr;

		CreateColors(other.m_colors != nullptr);
		if (other.m_colors) {
			memcpy(m_colors, other.m_colors, ColorCount());
		}

		return *this;
	}

	Texture& Texture::operator=(
		Texture&& other) noexcept
	{
		m_width        = other.m_width;
		m_height       = other.m_height;
		m_depth        = other.m_depth;
		m_channels     = other.m_channels;
		m_type         = other.m_type;
		m_format       = other.m_format;
		m_formatType   = other.m_formatType;
		m_wrap         = other.m_wrap;
		m_filter       = other.m_filter;
		m_mipmapFilter = other.m_mipmapFilter;
		m_parent       = other.m_parent;
		m_xOffset      = other.m_yOffset;
		m_yOffset      = other.m_yOffset;
		m_ownsColors   = other.m_ownsColors;
		m_colors       = other.m_colors;
		m_handle       = other.m_handle;

		other.m_colors = nullptr;
		other.m_handle = nullptr;

		return *this;
	}

	void Texture::Initialize(
		const iw::ref<IDevice>& device)
	{
		if (m_handle) {
			LOG_WARNING << "Texture already initialized!";
			return;
		}

		if (IsSubTexture()) {
			if (!m_parent->m_handle) {
				m_parent->Initialize(device);
			}

			m_handle = device->CreateSubTexture(m_parent->Handle(), m_xOffset, m_yOffset, m_width, m_height);
		}

		else { // no depth. for now it = height
			m_handle = device->CreateTexture(m_width, m_height, m_type, m_format, m_formatType, m_wrap, m_filter, m_mipmapFilter, m_colors);
		}
	}

	void Texture::Update(
		const iw::ref<IDevice>& device)
	{
		if (!m_handle) {
			Initialize(device);
		}

		device->SetTextureData(m_handle, m_handle, 0, 0, m_width, m_height, 0);
	}

	Texture Texture::CreateSubTexture(
		int xOffset, 
		int yOffset, 
		unsigned width,
		unsigned height/*,
		int mipmap = 0*/) /*const*/
	{
		return Texture(this, xOffset, yOffset, width, height/*, minmap*/);
	}

	void Texture::Clear(
		Color color)
	{
		if (m_handle) {
			m_handle->Clear(&color);
		}

		if (m_colors) {
			//switch (m_channels) {
			//case 1:
			//	break;
			//case 2:
			//	break;
			//case 3:
			//	break;
			//case 4:
			//	break;
			//}


			memset(m_colors, 0, ColorCount()); // get right color from arg
		}
	}

	void Texture::SetPixels(
		unsigned width,
		unsigned height,
		void* data) // assuming is null
	{
		m_width  = width;
		m_height = height;

		if (m_colors)
		{
			CreateColors(true); // realloc with dif sized colors array
		}

		if (data)
		{
			assert("no impl");
			// set from data
		}

		if (!m_handle) {
			LOG_DEBUG << "Tried to set pixels before initializing texture!";
			return;
		}

		m_handle->SetPixels(width, height, m_colors);
	}

	void Texture::SetBorderColor(
		Color color)
	{
		if (!m_handle) {
			return;
		}

		m_handle->SetBorderColor(&color);
	}

	void Texture::SetFilter(
		TextureFilter filter)
	{
		m_filter = filter;

		if (!m_handle) {
			return;
		}

		m_handle->SetFilter(filter);
	}

	void Texture::SetMipmapFilter(
		TextureMipmapFilter mipmapFilter)
	{
		m_mipmapFilter = mipmapFilter;

		if (!m_handle) {
			return;
		}

		m_handle->SetMipmapFilter(mipmapFilter);
	}

	unsigned Texture::Width() const {
		return m_width;
	}

	unsigned Texture::Height() const {
		return m_height;
	}

	unsigned Texture::Depth() const {
		return m_depth;
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

	TextureWrap Texture::Wrap() const {
		return m_wrap;
	}

	TextureFilter Texture::Filter() const {
		return m_filter;
	}

	TextureMipmapFilter Texture::MipmapFilter() const {
		return m_mipmapFilter;
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

	unsigned* Texture::Colors32() const
	{
		return (unsigned*)m_colors;
	}
	
	unsigned char* Texture::CreateColors(
		bool evenIfNotNull)
	{
		if (!m_colors || evenIfNotNull) {
			delete[] m_colors;

			m_ownsColors = true;
			m_colors = new unsigned char[ColorCount()];
			Clear();
		}

		return m_colors;
	}

	size_t Texture::ColorCount() const {
		return m_width * m_height * m_depth * m_channels; // convert everything to size_t 
	}

	size_t Texture::ColorCount32() const
	{
		return ColorCount() / 4;
	}

	ITexture* Texture::Handle() const {
		return m_handle;
	}

	bool Texture::IsSubTexture() const {
		return !!m_parent;
	}

	glm::vec2 Texture::Dimensions() const {
		return glm::vec2(m_width, m_height);
	}
}
}
