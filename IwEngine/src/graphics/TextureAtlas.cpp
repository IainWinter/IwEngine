#include "iw/graphics/TextureAtlas.h"

namespace iw {
namespace Graphics {
	TextureAtlas::TextureAtlas()
		: Texture()
	{}

	TextureAtlas::TextureAtlas(
		int width,
		int height,
		TextureFormat format,
		TextureFormatType type,
		TextureWrap wrap,
		TextureFilter filter,
		TextureMipmapFilter mipmapFilter,
		unsigned char* colors)
		: Texture(width, height, TEX_2D, format, type, wrap, filter, mipmapFilter, colors)
	{}

	TextureAtlas::TextureAtlas(
		const Texture& texture)
		: Texture(texture)
	{}

	TextureAtlas::TextureAtlas(
		Texture&& texture) noexcept
		: Texture(std::forward<Texture>(texture))
	{}

	glm::vec2 TextureAtlas::MapCoords(
		int tile,
		glm::vec2 coords) const
	{
		const TexBounds& bounds = m_bounds.at(tile);

		int x = bounds.X + bounds.Width  - bounds.X;
		int y = bounds.Y + bounds.Height - bounds.Y;

		return glm::vec2(x, y) * coords;
	}

	iw::ref<Texture> TextureAtlas::GetSubTexture(
		int tile)
	{
		if (m_bounds.size() == 0) // this is kinda bad GenTexBounds is a weird name for an init function, should pass in constructor maybe
		{
			LOG_WARNING << "[Graphics] Error in GetSubTexture, GenTexBounds has not been called!";
			return nullptr;
		}

		TexBounds bounds = m_bounds.at(tile);
		Texture tex = CreateSubTexture(
			bounds.X,     bounds.Y,
			bounds.Width, bounds.Height);

		iw::ref<Texture> tref = std::make_shared<Texture>(std::forward<Texture>(tex));

		return m_textures.emplace_back(tref);
	}

	TexBounds TextureAtlas::GetTexBounds(
		int tile) const
	{
		return m_bounds.at(tile);
	}

	//void TextureAtlas::GenTexBounds(
	//	iw::glm::vec4 backgroundColor)
	//{
	//	// algo for finding texture bounds
	//}

	void TextureAtlas::GenTexBounds(
		int cols,
		int rows)
	{
		m_bounds.clear();

		int xstep = m_width  / cols;
		int ystep = m_height / rows;
		for (int r = 0; r < rows; r++) {
			for (int c = 0; c < cols; c++) {
				int x = c * xstep;
				int y = r * xstep;

				m_bounds.emplace_back(
					x, y,
					(c + 1) * xstep - x, 
					(r + 1) * ystep - y
				);
			}
		}
	}

	//const std::vector<TexBounds>& TextureAtlas::Textures() const {
	//	return m_textures;
	//}

	int TextureAtlas::TileCount() const {
		return m_bounds.size();
	}
}
}
