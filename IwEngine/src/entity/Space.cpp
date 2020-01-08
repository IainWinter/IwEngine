#pragma once

#include "iw/entity/Space.h"
//#include "iw/log/logger.h"
#include <assert.h>

namespace IW {
	iw::ref<Component>& Space::RegisterComponent(
		ComponentType type,
		size_t size)
	{
		return m_componentManager.RegisterComponent(type, size);
	}

	iw::ref<Component> Space::GetComponent(
		ComponentType type)
	{
		return m_componentManager.GetComponent(type);
	}

	iw::ref<ComponentQuery> Space::MakeQuery(
		std::initializer_list<iw::ref<Component>> components)
	{
		size_t bufSize = sizeof(ComponentQuery)
			+ sizeof(iw::ref<Component>)
			* components.size();

		ComponentQuery* buf = (ComponentQuery*)malloc(bufSize);
		assert(buf);
		memset(buf, 0, bufSize);

		buf->Count = components.size();

		size_t i = 0;
		for (iw::ref<Component> component : components) {
			buf->Components[i++] = component;
		}

		return iw::ref<ComponentQuery>(buf, free);
	}

	iw::ref<Archetype>& Space::CreateArchetype(
		std::initializer_list<iw::ref<Component>> components)
	{
		return m_archetypeManager.CreateArchetype(components);
	}

	Entity Space::CreateEntity(
		const iw::ref<Archetype>& archetype)
	{
		iw::ref<EntityData>& entityData = m_entityManager.CreateEntity();
		bool componentsExist = false;
		if (entityData->Archetype && entityData->Archetype->Hash == archetype->Hash) {
			componentsExist = m_componentManager.ReinstateEntityComponents(entityData);
			//LOG_DEBUG << "Recycling  components " << entityData->ChunkIndex << " for " << entityData->Entity.Index;
		}

		else {
			entityData->Archetype = archetype;
		}

		if (!componentsExist) {
			entityData->ChunkIndex = m_componentManager.ReserveEntityComponents(entityData);
			//LOG_DEBUG << "Creating   components " << entityData->ChunkIndex << " for " << entityData->Entity.Index;
		}

		return entityData->Entity;
	}

	bool Space::DestroyEntity(
		size_t index)
	{
		iw::ref<EntityData>& entityData = m_entityManager.GetEntityData(index);
		if (!entityData->Entity.Alive) {
			LOG_WARNING << "Tried to delete dead entity!";
			return false;
		}

		//LOG_DEBUG << "Destroying components " << entityData->ChunkIndex << " for " << entityData->Entity.Index;

		return m_entityManager.DestroyEntity(index)
			&& m_componentManager.DestroyEntityComponents(entityData);
	}

	EntityComponentArray Space::Query(
		const iw::ref<ComponentQuery>& query)
	{
		return m_componentManager.Query(query, m_archetypeManager.MakeQuery(query));
	}
}
