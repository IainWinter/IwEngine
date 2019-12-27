#pragma once

#include "IwGraphics.h"
#include "Texture.h"
#include <vector>

namespace IW {
namespace Graphics {
	struct TexBounds {
		iw::vector2 Min;
		iw::vector2 Max;

		TexBounds(
			iw::vector2 min,
			iw::vector2 max)
			: Min(min)
			, Max(max)
		{}
	};

	struct TextureAtlas
		: Texture
	{
	private:
		std::vector<TexBounds> m_textures;

	public:
		IWGRAPHICS_API
		TextureAtlas();

		IWGRAPHICS_API
		TextureAtlas(
			int width,
			int height,
			TextureFormat format,
			TextureFormatType type,
			unsigned char* colors = nullptr);

		IWGRAPHICS_API
		TextureAtlas(
			const Texture& texture);

		IWGRAPHICS_API
		TextureAtlas(
			Texture&& texture) noexcept;

		GEN_default5(IWGRAPHICS_API, TextureAtlas)

		IWGRAPHICS_API
		iw::vector2 MapCoords(
			int tile,
			iw::vector2 coords) const;

		Texture GetSubTexture(
			int tile) const;

		IWGRAPHICS_API
		TexBounds GetTexBounds(
			int tile) const;

		// Generate bounds based on largest areas without the 'backgroundColor' surrounding
		IWGRAPHICS_API
		void GenTexBounds(
			iw::vector4 backgroundColor);

		// Split texture into equal squares
		IWGRAPHICS_API
		void GenTexBounds(
			int cols,
			int rows);

		// Split a tile into subtiles
		IWGRAPHICS_API
		void GenSubTexBounds(
			int tile,
			int cols,
			int rows);

		IWGRAPHICS_API
		const std::vector<TexBounds>& Textures() const;

		IWGRAPHICS_API
		int TileCount() const;
	};
}
}
