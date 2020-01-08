#include "iw/graphics/TextureAtlas.h"

namespace IW {
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
		unsigned char* colors)
		: Texture(width, height, format, type, wrap, colors)
	{}

	TextureAtlas::TextureAtlas(
		const Texture& texture)
		: Texture(texture)
	{}

	TextureAtlas::TextureAtlas(
		Texture&& texture) noexcept
		: Texture(std::forward<Texture>(texture))
	{}

	iw::vector2 TextureAtlas::MapCoords(
		int tile,
		iw::vector2 coords) const
	{
		const TexBounds& bounds = m_bounds.at(tile);

		int x = bounds.X + bounds.Width  - bounds.X;
		int y = bounds.Y + bounds.Height - bounds.Y;

		return iw::vector2(x, y) * coords;
	}

	iw::ref<Texture> TextureAtlas::GetSubTexture(
		int tile)
	{
		// check if algreay gotten

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
	//	iw::vector4 backgroundColor)
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
