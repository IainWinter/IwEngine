#pragma once

#include "IwEntity.h"
#include "Entity.h"
#include <vector>
#include <queue>

namespace IwEntity {
	class IWENTITY_API EntityManager {
	private:
		std::vector<Entity2> m_entities;
		std::queue<size_t>  m_dead;

	public:
		Entity2& CreateEntity(
			std::weak_ptr<Archetype2> archetype);

		bool DestroyEntity(
			size_t index);

		Entity2& GetEntity(
			size_t entityIndex);
	};
}
