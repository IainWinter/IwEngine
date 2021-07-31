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

	m_bounds = AABB2(glm::vec2(0.f), m_sprite->Dimensions() /*- glm::vec2(1.f)*/);

	m_polygon = MakePolygonFromBounds(m_bounds);
	m_index   = TriangulatePolygon(m_polygon);

	m_uv = m_polygon; // uv origin is 0

	glm::vec2 halfSize = m_sprite->Dimensions() / 2.0f;

	for (glm::vec2& v : m_collider) v -= halfSize ; // Origins to middle
	for (glm::vec2& v : m_polygon)  v -= halfSize ;

	m_bounds.Min -= halfSize; // could make better flow
	m_bounds.Max -= halfSize;
}

void Tile::UpdateColliderPolygon()
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

	RemoveTinyTriangles(m_collider, m_colliderIndex, 0.01f); // find best value for this, colliders can be less detailed
}

IW_PLUGIN_SAND_END
