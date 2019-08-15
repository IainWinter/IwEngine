#pragma once

#include "IwEntity.h"
#include "ComponentArray.h"
#include "EntityArray.h"
#include "EntityData.h"
#include "ArchetypeArray.h"
#include "Archetype.h"
#include "View.h"
#include "iw/util/set/sparse_set.h"
#include <unordered_map>
#include <cstddef>
#include <malloc.h>
#include <typeindex>
#include <vector>
#include <algorithm>
#include <initializer_list>

namespace IwEntity2 {
	class Space {
	private:
		EntityArray    m_entities;
		ArchetypeArray m_archetypes;
		std::vector<ComponentArray*> m_components;
		std::unordered_map<std::type_index, ComponentType> m_componentTypes;

	public:
		IWENTITY2_API Entity CreateEntity();

		IWENTITY2_API bool DestroyEntity(
			Entity entity);

		IWENTITY2_API bool EntityExists(
			Entity entity);

		template<
			typename _c>
		_c& CreateComponent(
			Entity entity)
		{
			ComponentType cid = GetComponentId(typeid(_c));
			return *(_c*)CreateComponent(entity, sizeof(_c), cid);
		}

		template<
			typename _c>
		bool DestroyComponent(
			Entity entity)
		{
			ComponentType cid = GetComponentId(typeid(_c));
			return DestroyComponent(entity, cid);
		}

		template<
			typename _c>
		_c& GetComponent(
			Entity entity)
		{
			ComponentType cid = GetComponentId(typeid(_c));
			return *(_c*)GetComponent(entity, cid);
		}

		template<
			typename... _c>
		View ViewComponents() {
			return ViewComponents({
				GetComponentId(typeid(_c))...
			});
		}
	private:
		IWENTITY2_API void* CreateComponent(
			Entity entity,
			size_t componentSize,
			ComponentType componentType);

		IWENTITY2_API bool DestroyComponent(
			Entity entity,
			ComponentType componentType);

		IWENTITY2_API void* GetComponent(
			Entity entity,
			ComponentType componentType);

		IWENTITY2_API View ViewComponents(
			std::initializer_list<ComponentType> ids);

		IWENTITY2_API ComponentArray* GetComponentArray(
			EntityArchetype archetype);

		IWENTITY2_API ComponentArray& EnsureComponentArray(
			EntityArchetype archetype);

		IWENTITY2_API ComponentType GetComponentId(
			std::type_index&& type);
	};
}
