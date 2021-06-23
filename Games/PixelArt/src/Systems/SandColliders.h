#pragma once

#include "plugins/iw/Sand/SandWorld.h"
#include "iw/engine/System.h"
#include "iw/physics/Collision/MeshCollider.h"
#include "iw/util/algorithm/pair_hash.h"
#include "iw/common/algos/polygon2.h"
#include <unordered_map>
#include <functional>

class SandColliderSystem : public iw::SystemBase
{
public:
	using madeCollider_func = std::function<void(iw::Entity)>;
	using cutCollider_func = std::function<void(iw::MeshCollider2*, iw::polygon_cut&)>;

	madeCollider_func MadeColliderCallback;
	cutCollider_func  CutColliderCallback;

private:
	iw::SandWorld* m_world;
	std::unordered_map<
		std::pair<int, int>,
		std::pair<std::vector<iw::EntityHandle>, size_t>,
		iw::pair_hash
	> m_cachedColliders;

	int m_cellsPerMeter;

public:
	SandColliderSystem(
		iw::SandWorld* world,
		int cellsPerMeter
	)
		: iw::SystemBase("Scan Colliders")
		, m_world(world)
		, m_cellsPerMeter(cellsPerMeter)
	{}

	void Update() override;
	void CutWorld(const glm::vec2& a, const glm::vec2& b);
};
