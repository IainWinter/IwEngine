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

	auto polygons = MakePolygonFromField(colors, size.x, size.y, 1u);
	
	if (polygons.size() == 0) return;

	//for (glm::vec2& p : polygons[0]) // ideally this would just be smushed into 1, making accessor functions for polygon2 would make this possible
	//{
	//	m_polygon.emplace_back(
	//		ceil(p.x - size.x / 2), ceil(p.y - size.y / 2),
	//		ceil(p.x),              ceil(p.y)
	//	);
	//}

	m_polygon = polygons[0]; // maybe combine into single polygon?
	m_index   = TriangulatePolygon(polygons[0]);

	m_uv = m_polygon;

	for (glm::vec2& v : m_polygon) {
		v -= size / 2.0f;
	}

	m_bounds = GenPolygonBounds(m_polygon);
}

IW_PLUGIN_SAND_END
