#pragma once

#include "IwEntity.h"
#include "Entity.h"
#include <vector>
#include <queue>

namespace IwEntity2 {
	class IWENTITY2_API EntityManager {
	private:
		std::vector<Entity>     m_entities;
		std::queue<EntityIndex> m_dead;

	public:
		EntityIndex CreateEntity();

		bool DestroyEntity(
			EntityIndex entity);

		Entity& GetEntityData(
			EntityIndex entity);

		bool EntityExists(
			EntityIndex entity) const;
	};
}
