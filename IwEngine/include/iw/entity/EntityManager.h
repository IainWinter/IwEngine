#pragma once

#include "IwEntity.h"
#include "EntityData.h"
#include <vector>
#include <deque>

namespace IwEntity {
	class IWENTITY_API EntityManager {
	private:
		std::vector<iwu::ref<EntityData>> m_entities;
		std::deque<size_t> m_dead;

	public:
		iwu::ref<EntityData>& CreateEntity();

		bool DestroyEntity(
			const Entity& entity);

		iwu::ref<EntityData>& GetEntityData(
			const Entity& entity);
	};
}
