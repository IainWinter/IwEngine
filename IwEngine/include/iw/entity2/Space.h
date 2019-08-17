#pragma once

#include "IwEntity.h"
#include "ComponentArray.h"
#include "EntityManager.h"
#include "ArchetypeManager.h"
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
		EntityManager    m_entityManager;
		ArchetypeManager m_archetypeManager;
		std::vector<ComponentManager*> m_components;
		std::unordered_map<std::type_index, ComponentType> m_componentTypes;

	public:
		IWENTITY2_API EntityIndex CreateEntity();

		IWENTITY2_API bool DestroyEntity(
			EntityIndex entity);

		IWENTITY2_API bool EntityExists(
			EntityIndex entity);

		template<
			typename _c>
		_c& CreateComponent(
			EntityIndex entity)
		{
			ComponentType cid = GetComponentId(typeid(_c));
			return *(_c*)CreateComponent(entity, sizeof(_c), cid);
		}

		template<
			typename _c>
		bool DestroyComponent(
			EntityIndex entity)
		{
			ComponentType cid = GetComponentId(typeid(_c));
			return DestroyComponent(entity, cid);
		}

		template<
			typename _c>
		_c& GetComponent(
			EntityIndex entity)
		{
			ComponentType cid = GetComponentId(typeid(_c));
			return *(_c*)GetComponent(entity, cid);
		}

		template<
			typename... _c>
		View ViewComponents() {
			Archetype query(
				ComponentTypeInfo {
					GetComponentId(typeid(_c)),
					sizeof(_c)
				}...
			);

			return ViewComponents(query);
		}
	private:
		IWENTITY2_API void* CreateComponent(
			EntityIndex entity,
			size_t componentSize,
			ComponentType componentType);

		IWENTITY2_API bool DestroyComponent(
			EntityIndex entity,
			ComponentType componentType);

		IWENTITY2_API void* GetComponent(
			EntityIndex entity,
			ComponentType componentType);

		IWENTITY2_API View ViewComponents(
			const Archetype& query);

		IWENTITY2_API ComponentManager* GetComponentArray(
			EntityArchetype archetype);

		IWENTITY2_API ComponentManager& EnsureComponentArray(
			EntityArchetype archetype);

		IWENTITY2_API ComponentType GetComponentId(
			std::type_index&& type);
	};
}
