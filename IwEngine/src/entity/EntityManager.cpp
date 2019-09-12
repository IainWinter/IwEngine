#include "iw/entity/EntityManager.h"
#include <assert.h>

namespace IwEntity {
	iwu::ref<Entity2> EntityManager::CreateEntity() {
		if (!m_dead.empty()) {
			iwu::ref<Entity2>& dead = m_entities.at(m_dead.front());
			m_dead.pop();

			dead->Alive = true;
			dead->Version++;
			dead->Archetype.reset();
			dead->ComponentData.reset();

			return dead;
		}

		size_t bufSize = sizeof(Entity2);

		Entity2* entity = (Entity2*)malloc(bufSize); //todo: memory allocation
		assert(entity);
		memset(entity, 0, bufSize);

		entity->Index = m_entities.size();
		entity->Alive = true;

		return m_entities.emplace_back(entity);
	}

	bool EntityManager::DestroyEntity(
		size_t entityIndex)
	{
		iwu::ref<Entity2>& dead = m_entities.at(entityIndex);
		if (dead->Alive) {
			dead->Alive = false;
			m_dead.push(dead->Index);

			return true;
		}

		return false;
	}
}
