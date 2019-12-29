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
		const TexBounds& bounds = m_bounds.at(tile);
		return bounds.Offset + bounds.Dimensions - bounds.Offset * coords;
	}

	iw::ref<Texture> TextureAtlas::GetSubTexture(
		int tile)
	{
		// check if algreay gotten

		TexBounds bounds = m_bounds.at(tile);
		Texture tex = CreateSubTexture(
			bounds.Offset.x,     bounds.Offset.y,
			bounds.Dimensions.x, bounds.Dimensions.y);

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

		float xstep = m_width / cols;
		float ystep = m_height / rows;
		for (int r = 0; r < rows; r++) {
			for (int c = 0; c < cols; c++) {
				iw::vector2 min( c      * xstep,  r      * ystep);
				iw::vector2 max((c + 1) * xstep, (r + 1) * ystep);

				m_bounds.emplace_back(
					min, 
					max - min
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
