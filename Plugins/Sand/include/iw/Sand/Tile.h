#pragma once

#include "SandChunk.h"
#include "iw/graphics/Texture.h"
#include "iw/common/Components/Transform.h"
#include "iw/physics/AABB.h"
#include <functional>
#include <utility>

IW_PLUGIN_SAND_BEGIN

struct Tile {
	ref<Texture> m_sprite;

	// It seems like the raster fidelity is better if the rendererd geometry is just a square
	// The physical colliders should still use scanned one obviously, but many more triangles can be removed

	std::vector<glm::vec2> m_polygon; // for raster
	std::vector<glm::vec2> m_uv;
	std::vector<unsigned>  m_index;

	std::vector<glm::vec2> m_collider; // for physics
	std::vector<unsigned>  m_colliderIndex;

	AABB2 m_bounds;

	Transform LastTransform;
	bool IsStatic = false;
	bool NeedsDraw = true;
	bool NeedsScan = true;

	IW_PLUGIN_SAND_API
	Tile() = default;

	IW_PLUGIN_SAND_API
	Tile(ref<Texture> texture);

	IW_PLUGIN_SAND_API
	void UpdateColliderPolygon();
};

IW_PLUGIN_SAND_END
