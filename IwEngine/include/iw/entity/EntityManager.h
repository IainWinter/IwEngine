#pragma once

#include "IwEntity.h"
#include "EntityData.h"
#include "iw/util/memory/pool_allocator.h"
#include <vector>
#include <queue>

namespace IW {
namespace ECS {
	class EntityManager {
	private:
		std::vector<iw::ref<EntityData>> m_entities;
		std::priority_queue<size_t, std::vector<size_t>, std::greater<size_t>> m_dead;
		iw::pool_allocator m_pool;

	public:
		EntityManager();

		iw::ref<EntityData>& CreateEntity();

		bool DestroyEntity(
			size_t index);

		IWENTITY_API
		iw::ref<EntityData>& GetEntityData(
			size_t index);
	};
}

	using namespace ECS;
}
