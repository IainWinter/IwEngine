#include "iw/entity/EntityManager.h"
#include <assert.h>

namespace IwEntity {
	std::weak_ptr<Entity2> EntityManager::CreateEntity(
		std::weak_ptr<Archetype2> archetype)
	{
		if (!m_dead.empty()) {
			auto dead = m_entities.at(m_dead.front());
			m_dead.pop();

			dead->ChunkIndex = 0;
			dead->Alive = true;
			dead->Version++;
			dead->Archetype = archetype;
			dead->ComponentData.reset();

			return dead;
		}

		size_t bufSize = sizeof(Entity2);

		Entity2* buf = (Entity2*)malloc(bufSize); //todo: memory allocation
		assert(buf);
		memset(buf, 0, bufSize);

		buf->Index = m_entities.size();
		buf->Alive = true;
		buf->Archetype = archetype;

		return m_entities.emplace_back(buf);
	}

	bool EntityManager::DestroyEntity(
		std::weak_ptr<Entity2> entity)
	{
		auto dead = m_entities.at(entity.lock()->Index); // Make sure this entity is in the list probly needed?
		if (dead->Alive) {
			dead->Alive = false;
			m_dead.push(dead->Index);

			return true;
		}

		return false;
	}

	std::weak_ptr<Entity2> EntityManager::GetEntity(
		size_t entityIndex)
	{
		return m_entities.at(entityIndex);
	}
}
