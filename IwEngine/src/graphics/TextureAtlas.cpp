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
		unsigned char* colors)
		: Texture(width, height, format, type, colors)
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
		const TexBounds& bounds = m_textures.at(tile);
		return bounds.Min / bounds.Max + coords * (bounds.Max - bounds.Min) / Dimensions();
	}

	Texture TextureAtlas::GetSubTexture(
		int tile) const
	{
		return ;
	}

	TexBounds TextureAtlas::GetTexBounds(
		int tile) const
	{
		return m_textures.at(tile);
	}

	void TextureAtlas::GenTexBounds(
		iw::vector4 backgroundColor)
	{
		// algo for finding texture bounds
	}

	void TextureAtlas::GenTexBounds(
		int cols,
		int rows)
	{
		m_textures.clear();

		float xstep = Width / cols;
		float ystep = Height / rows;
		for (int r = 0; r < rows; r++) {
			for (int c = 0; c < cols; c++) {
				m_textures.emplace_back(
					iw::vector2(c * xstep, r * ystep), 
					iw::vector2((c + 1) * xstep, (r + 1) * ystep)
				);
			}
		}
	}

	const std::vector<TexBounds>& TextureAtlas::Textures() const {
		return m_textures;
	}

	int TextureAtlas::TileCount() const {
		return m_textures.size();
	}
}
}
