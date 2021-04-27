#pragma once

#include "IwGraphics.h"
#include "Texture.h"
#include <vector>

namespace iw {
namespace Graphics {
	struct TexBounds {
		int X;
		int Y;
		int Width;
		int Height;

		TexBounds(
			int x,
			int y,
			int width,
			int height)
			: X(x)
			, Y(y)
			, Width(width)
			, Height(height)
		{}
	};

	struct TextureAtlas
		: Texture
	{
	private:
		std::vector<TexBounds> m_bounds;
		std::vector<iw::ref<Texture>> m_textures;

	public:
		IWGRAPHICS_API
		TextureAtlas();

		IWGRAPHICS_API
		TextureAtlas(
			int width,
			int height,
			TextureFormat format             = RGBA,
			TextureFormatType type           = UBYTE,
			TextureWrap wrap                 = BORDER,
			TextureFilter filter             = NEAREST,
			TextureMipmapFilter mipmapFilter = NEAREST_NEAREST,
			unsigned char* colors            = nullptr);

		IWGRAPHICS_API
		TextureAtlas(
			const Texture& texture);

		IWGRAPHICS_API
		TextureAtlas(
			Texture&& texture) noexcept;

		GEN_default5(IWGRAPHICS_API, TextureAtlas)

		IWGRAPHICS_API
		glm::vec2 MapCoords(
			int tile,
			glm::vec2 coords) const;

		IWGRAPHICS_API
		iw::ref<Texture> GetSubTexture(
			int tile);

		IWGRAPHICS_API
		TexBounds GetTexBounds(
			int tile) const;

		//// Generate bounds based on largest areas without the 'backgroundColor' surrounding
		//IWGRAPHICS_API
		//void GenTexBounds(
		//	iw::glm::vec4 backgroundColor);

		// Split texture into equal rectangles
		IWGRAPHICS_API
		void GenTexBounds(
			int cols,
			int rows);

		//// Split a tile into subtiles
		//IWGRAPHICS_API
		//void GenSubTexBounds(
		//	int tile,
		//	int cols,
		//	int rows);

		//IWGRAPHICS_API
		//const std::vector<TexBounds>& TextureBounds() const;

		IWGRAPHICS_API
		int TileCount() const;
	};
}
}
