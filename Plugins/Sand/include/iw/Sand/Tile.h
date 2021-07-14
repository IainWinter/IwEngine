#pragma once

#include "Cell.h"
#include "iw/asset/AssetManager.h"
#include "iw/physics/Collision/MeshCollider.h"
#include "iw/physics/AABB.h"
#include "iw/graphics/Mesh.h"
#include <functional>
#include <utility>

IW_PLUGIN_SAND_BEGIN

struct Tile {
	//Mesh m_spriteMesh;
	ref<Texture> m_sprite;

	std::vector<glm::vec2> m_polygon;
	std::vector<glm::vec2> m_uv;
	std::vector<unsigned>  m_index;

	//iw::AABB2 m_bounds;

	Transform LastTransform;
	bool IsStatic = false;
	bool NeedsDraw = true;
	bool NeedsScan = true;

	Tile() = default;
	Tile(ref<Texture> texture, ref<Shader> shader);

	void UpdatePolygon();
};

IW_PLUGIN_SAND_END
