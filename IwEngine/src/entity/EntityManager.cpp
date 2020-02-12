#include "iw/entity/EntityManager.h"
#include <assert.h>

namespace iw {
namespace ECS {
	EntityManager::EntityManager()
		: m_pool(1024)
	{}

	iw::ref<EntityData>& EntityManager::CreateEntity() {
		if (!m_dead.empty()) {
			iw::ref<EntityData>& dead = m_entities.at(m_dead.top());
			m_dead.pop();

			dead->Entity.Alive = true;
			dead->Entity.Version++;

			return dead;
		}

		iw::ref<EntityData> entityData = m_pool.alloc_ref_t<EntityData>();

		entityData->Entity = EntityHandle {
			m_entities.size(),
			0,
			true
		};

		return m_entities.emplace_back(entityData);
	}

	bool EntityManager::DestroyEntity(
		size_t index)
	{
		iw::ref<EntityData>& dead = m_entities.at(index);
		if (dead->Entity.Alive) {
			dead->Entity.Alive = false;
			//dead->Archetype.reset();
			////dead->Components.reset();

			// Should clean up if too many dead entities stick around
			// Probly make it so each time a entity is created it also frees a dead entity from the back of the queue
			// Todo: Test which way is best to free the entities and chunks

			//if (m_dead.size() < 1000) {
				m_dead.push(dead->Entity.Index);
			//}

			//else {
			//	iw::ref<Entity2>& back = m_entities.back();
			//	
			//	back->Index = dead->Index;
			//	m_entities.at(entityIndex) = back;
			//	
			//	back = dead;

			//	m_entities.pop_back();
			//}

			return true;
		}

		return false;
	}

	iw::ref<EntityData>& EntityManager::GetEntityData(
		size_t index)
	{
		return m_entities.at(index);
	}

	void EntityManager::Clear() {
		m_entities.clear();
		while (!m_dead.empty()) m_dead.pop(); // clear pq
		m_pool.reset();
	}
}
}
