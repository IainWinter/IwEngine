#pragma once

#include "iw/entity2/IwEntity.h"
#include "iw/entity2/EntityData.h"
#include <vector>
#include <queue>

namespace IwEntity2 {
	class EntityArray {
	private:
		std::vector<EntityData> m_live;
		std::queue<size_t>      m_dead;

	public:
		EntityData& CreateEntity();

		EntityData& GetEntityData(
			Entity entity);

		bool DestroyEntity(
			Entity entity);

		bool EntityExists(
			Entity entity) const;
	};
}
