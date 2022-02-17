#include "iw/graphics/TextureAtlas.h"

namespace iw {
namespace Graphics {
	
	TextureAtlas::TextureAtlas()
		: m_tileCountX (1)
		, m_tileCountY (1)
		, m_texture    (nullptr)
	{
		UpdateTileScale();
	}

	TextureAtlas::TextureAtlas(
		int tileCountX,
		int tileCountY,
		ref<Texture> texture
	)
		: m_tileCountX (tileCountX)
		, m_tileCountY (tileCountY)
		, m_texture    (texture)
	{
		UpdateTileScale();
	}

	void TextureAtlas::UpdateTileScale()
	{
		m_tileScale.x = 1.f / m_tileCountX;
		m_tileScale.y = 1.f / m_tileCountY;
	}

	int TextureAtlas::TileCount() const
	{
		return m_tileCountX * m_tileCountY;
	}

	// column major
	int TextureAtlas::TileX(
		int tileIndex) const
	{
		return tileIndex % m_tileCountX;
	}

	int TextureAtlas::TileY(
		int tileIndex) const
	{
		return tileIndex / m_tileCountX;
	}

	glm::vec2 TextureAtlas::MapUv(
		int tileIndex,
		glm::vec2 uv) const
	{
		return MapUv(TileX(tileIndex), TileY(tileIndex), uv);
	}

	glm::vec2 TextureAtlas::MapUv(
		int tileX,
		int tileY,
		glm::vec2 uv) const
	{
		return (glm::vec2(tileX, tileY) + uv) * m_tileScale;
	}
}
}
