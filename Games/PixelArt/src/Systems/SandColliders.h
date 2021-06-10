#pragma once

#include "iw/engine/Layer.h"
#include "iw/physics/Collision/MeshCollider.h"
#include "iw/util/algorithm/pair_hash.h"
#include "plugins/iw/Sand/SandWorld.h"
#include <unordered_map>
#include <functional>

class SandColliderSystem : public iw::SystemBase
{
public:
	using updatedCollider_func = std::function<void(iw::Entity)>;

private:
	iw::SandWorld* m_world;
	std::unordered_map<
		std::pair<int, int>,
		std::pair<std::vector<iw::EntityHandle>, size_t>,
		iw::pair_hash
	> m_cachedColliders;

	updatedCollider_func m_callback;
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

	void SetCallback(updatedCollider_func func) {
		m_callback = func;
	}
};
