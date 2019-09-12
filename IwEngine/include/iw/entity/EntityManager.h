#pragma once

#include "IwEntity.h"
#include "Entity.h"
#include <memory>
#include <vector>
#include <queue>

namespace IwEntity {
	class IWENTITY_API EntityManager {
	private:
		std::vector<iwu::ref<Entity2>> m_entities;
		std::queue<size_t> m_dead;

	public:
		iwu::ref<Entity2> CreateEntity();

		bool DestroyEntity(
			size_t entityIndex);
	};
}
