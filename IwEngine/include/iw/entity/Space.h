#pragma once

#include "IwEntity.h"
#include "View.h"
#include "EntityData.h"
#include "ComponentData.h"
#include "iw/util/set/sparse_set.h"
#include "iw/util/type/family.h"
#include <assert.h>
#include <vector>
#include <tuple>
#include <bitset>
#include <cmath>

#include "iw/log/logger.h"

namespace IwEntity {
	class IWENTITY_API Space {
	private:
		std::vector<IComponentData*> m_components;
		EntityData m_entities;

	public:
		Entity CreateEntity() {
			return m_entities.CreateEntity();
		}

		void DestroyEntity(
			Entity entity)
		{
			m_entities.DestroyEntity(entity);

			for (auto& set : m_components) {
				if (set->HasEntity(entity)) {
					set->DestroyComponent(entity);
				}
			}
		}

		template<
			typename _c,
			typename... _args_t>
		_c& CreateComponent(
			Entity entity,
			_args_t&&... args)
		{
			Archetype oldArchetype = m_entities.ArchetypeOf(entity);
			Archetype& archetype   = m_entities.AssignComponent<_c>(entity);

			UpdateComponentData(entity, archetype, oldArchetype);

			return EnsureComponentData<_c>()
				.CreateComponent(
					entity,
					archetype,
					std::forward<_args_t>(args)...);
		}

		template<
			typename _c>
		void DestroyComponent(
			Entity entity)
		{
			if (ComponentDataExists<_c>()) {
				Archetype oldArchetype = m_entities.ArchetypeOf(entity);
				Archetype& archetype
					= m_entities.UnassignComponent<_c>(entity);

				UpdateComponentData(entity, archetype, oldArchetype);

				GetComponentData<_c>()
					->DestroyComponent(entity, archetype);
			}
		}

		//template<
		//	typename... _cs>
		//View<_cs...> ViewComponents() {
		//	Archetype archetype = GetArchetype<_cs...>();
		//	return View<_cs...>(
		//		GetSetBegin<_cs>(archetype)...,
		//		GetSetEnd  <_cs>(archetype)...);
		//}

		void Sort() {
			for (auto& set : m_components) {
				set->Sort(m_entities);
			}
		}
	private:
		template<
			typename _c>
		ComponentData<_c>& EnsureComponentData() {
			ComponentId id = ComponentFamily::type<_c>;
			if (id >= m_components.size()) {
				m_components.resize(id + 1);
			}

			IComponentData*& cdata = m_components.at(id);
			if (!cdata) {
				cdata = new ComponentData<_c>();
			}

			return *static_cast<ComponentData<_c>*>(cdata);
		}

		void UpdateComponentData(
			Entity entity,
			Archetype archetype,
			Archetype oldArchetype)
		{
			std::size_t index = 0;
			for (Archetype a = 1; a <= oldArchetype; a = a << 1, index++) {
				if ((oldArchetype & a) == a) {
					m_components.at(index)->UpdateChunk(
						entity, oldArchetype, archetype);
				}
			}
		}

		template<
			typename _c>
		ComponentData<_c>* GetComponentData() {
			return static_cast<ComponentData<_c>*>(
				m_components.at(ComponentFamily::type<_c>));
		}

		template<
			typename _c>
		bool ComponentDataExists() {
			return ComponentFamily::type<_c> < m_components.size();
		}
	};
}
