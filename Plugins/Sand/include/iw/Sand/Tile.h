#pragma once

#include "Cell.h"
#include "iw/graphics/Renderer.h" // Heavy includes...
#include "iw/asset/AssetManager.h"
#include "iw/physics/Collision/MeshCollider.h"
#include <functional>
#include <utility>

IW_PLUGIN_SAND_BEGIN

struct Tile {
private:
	Mesh m_spriteMesh;
	ref<Texture> m_sprite;
	ref<RenderTarget> m_target;
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
		MeshCollider* collider);

	void Draw(
		Transform* transform,
		Renderer* renderer);

	void ForEachInWorld(
		iw::Transform* transform,
		std::function<void(int, int, unsigned)> func);

	// Getters

	const ref<Texture>& GetSprite() const {
		return m_sprite;
	}

	const ref<Texture>& GetTarget() const {
		return IsStatic ? GetSprite() : m_target->Tex(0);
	}

	std::pair<glm::vec2*, size_t> GetPolygon() const {
		return {
			(glm::vec2*)m_spriteMesh.Data()->Get     (bName::POSITION),
					  m_spriteMesh.Data()->GetCount(bName::POSITION)
		};
	}

	std::pair<unsigned*, size_t> GetIndex() const {
		return {
			m_spriteMesh.Data()->GetIndex(),
			m_spriteMesh.Data()->GetIndexCount()
		};
	}
};

IW_PLUGIN_SAND_END
