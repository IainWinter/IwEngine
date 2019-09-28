#include "iw/entity/EntityManager.h"
#include <assert.h>

namespace IwEntity {
	iwu::ref<Entity> EntityManager::CreateEntity() {
		if (!m_dead.empty()) {
			iwu::ref<Entity>& dead = m_entities.at(m_dead.front());
			m_dead.pop_front();

			dead->Version++;

			return dead;
		}

		size_t bufSize = sizeof(Entity);

		Entity* entity = (Entity*)malloc(bufSize); //todo: memory allocation
		assert(entity);
		memset(entity, 0, bufSize);

		entity->Index = m_entities.size();

		return m_entities.emplace_back(entity);
	}

	bool EntityManager::DestroyEntity(
		size_t entityIndex)
	{
		iwu::ref<Entity> dead = m_entities.at(entityIndex);
		//if (dead->Alive) {
			//dead->Alive = false;
			//dead->Archetype.reset();
			////dead->Components.reset();

			// Should clean up if too many dead entities stick around
			// Probly make it so each time a entity is created it also frees a dead entity from the back of the queue
			// Todo: Test which way is best to free the entities and chunks

			//if (m_dead.size() < 1000) {
				m_dead.push_back(dead->Index);
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

			//return true;
		//}

		return false;
	}
}
