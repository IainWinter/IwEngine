#include "iw/Sand/Tile.h"
#include "iw/common/algos/polygon2.h"
#include "iw/physics/AABB.h"

IW_PLUGIN_SAND_BEGIN

Tile::Tile(
	ref<Texture> texture)
{
	m_sprite = texture;

	if (!m_sprite->m_colors) {
		m_sprite->SetFilter(iw::NEAREST);
		m_sprite->CreateColors();
	}
}

void Tile::UpdatePolygon()
{
	unsigned* colors = (unsigned*)m_sprite->Colors();
	glm::vec2 size   =            m_sprite->Dimensions();

	auto colliders = MakePolygonFromField<unsigned>( // why does this NEED template arg
		colors, 
		size.x, size.y,
		[](const unsigned& color)
		{
			return Color::From32(color).a > 0; // i wonder if this optimizes for only the alpha channel?
		});
	
	if (colliders.size() == 0) return;

	m_collider      = colliders[0]; // should combine into single polygon?
	m_colliderIndex = TriangulatePolygon(colliders[0]);

	m_bounds = GenPolygonBounds(m_collider);

	RemoveTinyTriangles(m_collider, m_colliderIndex, 0.1f); // find best value for this, colliders can be less detailed

	m_polygon = MakePolygonFromBounds(m_bounds);
	m_index   = TriangulatePolygon(m_polygon);

	m_uv = m_polygon; // uv origin is 0

	glm::vec2 halfSize = size / 2.0f;

	for (glm::vec2& v : m_collider) v -= halfSize ; // Origins to middle
	for (glm::vec2& v : m_polygon)  v -= halfSize ;

	m_bounds.Min -= halfSize; // could make better flow
	m_bounds.Max -= halfSize;
}

IW_PLUGIN_SAND_END
