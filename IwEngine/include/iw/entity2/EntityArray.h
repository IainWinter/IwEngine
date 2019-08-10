#pragma once

#include "IwEntity.h"
#include "EntityData.h"
#include "iw/util/memory/pool_allocator.h"
#include <vector>
#include <queue>

namespace IwEntity2 {
	class IWENTITY2_API EntityArray {
	private:
		std::vector<EntityData> m_entities;
		std::queue<Entity>      m_dead;

	public:
		Entity CreateEntity();

		bool DestroyEntity(
			Entity entity);

		EntityData& GetEntityData(
			Entity entity);

		bool EntityExists(
			Entity entity) const;
	};
}
