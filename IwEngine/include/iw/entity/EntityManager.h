#pragma once

#include "IwEntity.h"
#include "Entity.h"
#include <memory>
#include <vector>
#include <queue>

namespace IwEntity {
	class IWENTITY_API EntityManager {
	private:
		std::vector<std::shared_ptr<Entity2>> m_entities;
		std::queue<size_t> m_dead;

	public:
		std::weak_ptr<Entity2> CreateEntity(
			std::weak_ptr<Archetype2> archetype);

		bool DestroyEntity(
			std::weak_ptr<Entity2> index);

		std::weak_ptr<Entity2> GetEntity(
			size_t entityIndex);
	};
}
