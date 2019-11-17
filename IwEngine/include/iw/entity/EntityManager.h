#pragma once

#include "IwEntity.h"
#include "EntityData.h"
#include <vector>
#include <queue>

namespace IW {
inline namespace ECS {
	class IWENTITY_API EntityManager {
	private:
		std::vector<iw::ref<EntityData>> m_entities;
		std::priority_queue<size_t, std::vector<size_t>, std::greater<size_t>> m_dead;

	public:
		iw::ref<EntityData>& CreateEntity();

		bool DestroyEntity(
			size_t index);

		iw::ref<EntityData>& GetEntityData(
			size_t index);
	};
}
}
