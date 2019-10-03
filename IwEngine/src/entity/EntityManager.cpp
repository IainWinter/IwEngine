#include "iw/entity/EntityManager.h"
#include <assert.h>

namespace IwEntity {
	iwu::ref<EntityData>& EntityManager::CreateEntity() {
		if (!m_dead.empty()) {
			iwu::ref<EntityData>& dead = m_entities.at(m_dead.front());
			m_dead.pop_front();

			dead->Entity.Alive = true;
			dead->Entity.Version++;

			return dead;
		}

		size_t bufSize = sizeof(EntityData);

		EntityData* entityData = (EntityData*)malloc(bufSize); //todo: memory allocation
		assert(entityData);
		memset(entityData, 0, bufSize);

		entityData->Entity = Entity {
			m_entities.size(),
			0, 
			true
		};

		return m_entities.emplace_back(entityData, free);
	}

	bool EntityManager::DestroyEntity(
		size_t index)
	{
		iwu::ref<EntityData>& dead = m_entities.at(index);
		if (dead->Entity.Alive) {
			dead->Entity.Alive = false;
			//dead->Archetype.reset();
			////dead->Components.reset();

			// Should clean up if too many dead entities stick around
			// Probly make it so each time a entity is created it also frees a dead entity from the back of the queue
			// Todo: Test which way is best to free the entities and chunks

			//if (m_dead.size() < 1000) {
				m_dead.push_back(dead->Entity.Index);
			//}

			//else {
			//	iwu::ref<Entity2>& back = m_entities.back();
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

	iwu::ref<EntityData>& EntityManager::GetEntityData(
		size_t index)
	{
		return m_entities.at(index);
	}
}
