#pragma once

#include "IwEntity.h"
#include "EntityData.h"
#include "iw/util/memory/pool_allocator.h"
#include <vector>
#include <queue>

namespace iw {
namespace ECS {
	class EntityManager {
	private:
		std::vector<EntityData*> m_entities;
		std::priority_queue<size_t, std::vector<size_t>, std::greater<size_t>> m_dead;
		pool_allocator m_pool;

	public:
		IWENTITY_API
		EntityManager();

		IWENTITY_API
		EntityData& CreateEntity();

		IWENTITY_API
		bool DestroyEntity(
			size_t index);

		IWENTITY_API
		EntityData& GetEntityData(
			size_t index);

		IWENTITY_API
		void Clear();

		IWENTITY_API
		const std::vector<EntityData*>& Entities() const;

		IWENTITY_API
		bool IsValidEntityIndex(
			size_t index) const;
	};
}

	using namespace ECS;
}
