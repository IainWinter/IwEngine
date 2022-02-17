#pragma once

#include "IwGraphics.h"
#include "Texture.h"
#include <vector>

namespace iw {
namespace Graphics {

	// simple way to wrap a texture and generate UVs for smaller equal sized tiles
	//
	struct TextureAtlas
	{
		glm::vec2 m_tileScale; // if you resize the texture, you probally want to keep the same size tiles. Call UpdateTileScale if not.
		int m_tileCountX;
		int m_tileCountY;

		ref<Texture> m_texture; // dont really need this in here

		IWGRAPHICS_API TextureAtlas();
		IWGRAPHICS_API TextureAtlas(int tileCountX, int tileCountY, ref<Texture> texture);

		IWGRAPHICS_API void UpdateTileScale();
		IWGRAPHICS_API  int TileCount()        const;

		IWGRAPHICS_API int TileX(int tileIndex) const;
		IWGRAPHICS_API int TileY(int tileIndex) const;

		IWGRAPHICS_API glm::vec2 MapUv(int tileIndex,        glm::vec2 uv) const;
		IWGRAPHICS_API glm::vec2 MapUv(int tileX, int tileY, glm::vec2 uv) const;
	};
}
}
