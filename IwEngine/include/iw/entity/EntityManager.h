#pragma once

#include "IwEntity.h"
#include "Entity.h"
#include <vector>
#include <deque>

namespace IwEntity {
	class IWENTITY_API EntityManager {
	private:
		std::vector<iwu::ref<Entity>> m_entities;
		std::deque<size_t> m_dead;

	public:
		iwu::ref<Entity> CreateEntity();

		bool DestroyEntity(
			size_t entityIndex);
	};
}
