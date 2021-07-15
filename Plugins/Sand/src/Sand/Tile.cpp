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

	auto polygons = iw::MakePolygonFromField(colors, size.x, size.y, 1u);
	
	if (polygons.size() == 0) return;

	m_polygon = polygons[0]; // maybe combine into single polygon?
	m_index   = iw::TriangulatePolygon(m_polygon);

	for (glm::vec2& v : m_polygon) {
		v /= size;
	}

	m_uv = m_polygon;

	for (glm::vec2& v : m_polygon) {
		v = (v - glm::vec2(0.5f)) * size / 10.0f;
	}
}

IW_PLUGIN_SAND_END
