#include "iw/entity/EntityManager.h"
#include <assert.h>

namespace iw {
namespace ECS {
	EntityManager::EntityManager()
		: m_pool(1024, 2)
	{}

	EntityData& EntityManager::CreateEntity() {
		if (!m_dead.empty()) {
			EntityData* dead = m_entities.at(m_dead.top());
			m_dead.pop();

			dead->Entity.Alive = true;
			dead->Entity.Version++;

			return *dead;
		}

		EntityData* entityData = m_pool.alloc<EntityData>();
		entityData->Entity.Index   = m_entities.size();
		entityData->Entity.Version = 0;
		entityData->Entity.Alive   = true;

		m_entities.emplace_back(entityData);

		return *entityData;
	}

	bool EntityManager::DestroyEntity(
		size_t index)
	{
		EntityData& dead = GetEntityData(index);
		
		if (dead.Entity.Alive) 
		{
			dead.Entity.Alive = false;
			m_dead.push(dead.Entity.Index);

			return true;
		}

		return false;
	}

	EntityData& EntityManager::GetEntityData(
		size_t index)
	{
		return *m_entities.at(index);
	}

	void EntityManager::Clear() {
		while (!m_dead.empty()) m_dead.pop(); // clear pq
		m_entities.clear();
		m_pool.reset();
	}

	const std::vector<EntityData*>& EntityManager::Entities() const {
		return m_entities;
	}

	bool EntityManager::IsValidEntityIndex(
		size_t index) const
	{
		return index < m_entities.size();
	}
}
}
