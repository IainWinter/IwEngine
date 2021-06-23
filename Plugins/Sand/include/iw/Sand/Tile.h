#pragma once

#include "Cell.h"
#include "iw/graphics/Renderer.h" // Heavy includes...
#include "iw/asset/AssetManager.h"
#include "iw/physics/Collision/MeshCollider.h"
#include <functional>
#include <utility>

IW_PLUGIN_SAND_BEGIN

struct Tile {
//private:
	Mesh m_spriteMesh;
	ref<Texture> m_sprite;
	ref<RenderTarget> m_target;

	std::vector<glm::vec2> m_polygon;
	std::vector<unsigned>  m_index;

public:
	Transform LastTransform;
	bool IsStatic = false;
	bool NeedsDraw = true;
	bool NeedsScan = true;

	Tile() = default;

	Tile(
		ref<AssetManager>& asset,
		const std::string& spriteName,
		bool isStatic = false);

	void UpdatePolygon(
		float sx, float sy);

	void Draw(
		Transform* transform,
		Renderer* renderer);

	void ForEachInWorld(
		iw::Transform* transform,
		float sx, float sy,
		std::function<void(int, int, unsigned)> func);

	// Getters, remove these

	const ref<Texture>& GetSprite() const {
		return m_sprite;
	}

	const ref<Texture>& GetTarget() const {
		return IsStatic ? GetSprite() : m_target->Tex(0);
	}
};

IW_PLUGIN_SAND_END
