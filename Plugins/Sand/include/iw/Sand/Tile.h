#pragma once

#include "Cell.h"
#include "iw/graphics/Texture.h"
#include "iw/common/Components/Transform.h"
#include <functional>
#include <utility>

IW_PLUGIN_SAND_BEGIN

struct Tile {
	ref<Texture> m_sprite;

	std::vector<glm::vec2> m_polygon;
	std::vector<glm::vec2> m_uv;
	std::vector<unsigned>  m_index;

	Transform LastTransform;
	bool IsStatic = false;
	bool NeedsDraw = true;
	bool NeedsScan = true;

	Tile() = default;
	Tile(ref<Texture> texture);

	void UpdatePolygon();
};

IW_PLUGIN_SAND_END
