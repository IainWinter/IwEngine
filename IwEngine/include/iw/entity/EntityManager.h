#pragma once

#include "IwEntity.h"
#include "EntityData.h"
#include <vector>
#include <queue>

namespace IwEntity {
	class IWENTITY_API EntityManager {
	private:
		std::vector<iwu::ref<EntityData>> m_entities;
		std::priority_queue<size_t, std::vector<size_t>, std::greater<size_t>> m_dead;

	public:
		iwu::ref<EntityData>& CreateEntity();

		bool DestroyEntity(
			size_t index);

		iwu::ref<EntityData>& GetEntityData(
			size_t index);
	};
}
