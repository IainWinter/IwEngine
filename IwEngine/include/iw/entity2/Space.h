#pragma once

#include "IwEntity.h"
#include "ComponentArray.h"
#include "EntityArray.h"
#include "EntityData.h"
#include "ArchetypeArray.h"
#include "ArchetypeData.h"
#include "View.h"
#include "iw/util/set/sparse_set.h"
#include <unordered_map>
#include <cstddef>
#include <malloc.h>
#include <typeindex>
#include <vector>
#include <algorithm>

namespace IwEntity2 {
	class Space {
	private:
		EntityArray    m_entities;
		ArchetypeArray m_archetypes;
		std::unordered_map<std::type_index, Component> m_componentIds;
		iwu::sparse_set<Archetype, ComponentArray*>    m_components;

	public:
		IWENTITY2_API Entity CreateEntity();

		IWENTITY2_API bool DestroyEntity(
			Entity entity);

		template<
			typename _c>
		_c& CreateComponent(
			Entity entity)
		{
			Component cid = GetComponentId(typeid(_c));
			return *(_c*)CreateComponent(entity, sizeof(_c), cid);
		}

		template<
			typename _c>
		bool DestroyComponent(
			Entity entity)
		{
			Component cid = GetComponentId(typeid(_c));
			return DestroyComponent(entity, cid);
		}

		template<
			typename _c>
		_c& GetComponent(
			Entity entity)
		{
			Component cid = GetComponentId(typeid(_c));
			return *(_c*)GetComponent(entity, cid);
		}

		//template<
		//	typename... _c>
		//View ViewComponents() {
		//	//return View {
		//	//	//m_components.at(GetComponentId(typeid(_c)))...
		//	//};

		//	return View();
		//}

		IWENTITY2_API bool EntityExists(
			Entity entity);
	private:
		IWENTITY2_API void* CreateComponent(
			Entity entity,
			size_t componentSize,
			Component componentId);

		IWENTITY2_API bool DestroyComponent(
			Entity entity,
			Component componentId);

		IWENTITY2_API void* GetComponent(
			Entity entity,
			Component componentId);

		IWENTITY2_API View ViewComponents(
			std::initializer_list<Component> componentIds);

		IWENTITY2_API ComponentArray* GetComponentArray(
			Archetype archetype);

		IWENTITY2_API ComponentArray& EnsureComponentArray(
			Archetype archetype);

		IWENTITY2_API Component GetComponentId(
			std::type_index&& type);
	};
}
