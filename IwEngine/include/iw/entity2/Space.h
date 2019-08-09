#pragma once

#include "iw/entity2/IwEntity.h"
#include "iw/entity2/ComponentArray.h"
#include "iw/entity2/EntityArray.h"
#include "iw/entity2/Archetype.h"
#include "iw/entity2/EntityData.h"
#include "iw/util/memory/pool_allocator.h"
#include <unordered_map>
#include <cstddef>
#include <malloc.h>
#include <typeindex>
#include <vector>
#include <algorithm>

namespace IwEntity2 {
	class Space {
	private:
		EntityArray m_entities;
		std::unordered_map<Archetype, ComponentArray>  m_components;
		std::unordered_map<std::type_index, Component> m_componentIds;

	public:
		IWENTITY2_API Entity CreateEntity();

		IWENTITY2_API bool DestroyEntity(
			Entity entity);

		template<
			typename _c>
		_c* CreateComponent(
			Entity entity)
		{
			Component cid = GetComponentId(typeid(_c));
			return (_c*)CreateComponent(entity, sizeof(_c), cid);
		}

		template<
			typename _c>
		bool DestroyComponent(
			Entity entity)
		{
			Component cid = GetComponentId(typeid(_c));
			return DestroyComponent(entity, cid);
		}

		IWENTITY2_API bool EntityExists(
			Entity entity);
	private:
		IWENTITY2_API Component GetComponentId(
			std::type_index&& type);

		IWENTITY2_API void* CreateComponent(
			Entity entity,
			size_t componentSize,
			Component componentId);

		IWENTITY2_API bool DestroyComponent(
			Entity entity,
			Component componentId);

		IWENTITY2_API ComponentArray* GetComponentArray(
			const Archetype& archetype);

		IWENTITY2_API ComponentArray* FindComponentArray(
			Entity entity);

		IWENTITY2_API ComponentArray* CreateComponentArray(
			const Archetype& archetype,
			size_t archetypeSize);
	};
}
