#pragma once

#include "iw/util/set/sparse_set.h"
#include "iw/util/type/family.h"
#include <vector>
#include <tuple>

namespace IwEntity5 {
	class Space {
		using Entity          = unsigned int;
		using ComponentId     = unsigned int;
		using ComponentFamily = iwu::family<Space>;
		using ComponentSet    = iwu::sparse_set<ComponentId>;

		template<
			typename _component_t>
		using ComponentSetT = iwu::sparse_set<ComponentId, _component_t>;

	private:
		std::vector<ComponentSet*> m_components;

	public:
		template<
			typename _component_t,
			typename... _args_t>
		void CreateComponent(
			Entity entity,
			const _args_t&... args)
		{
			ComponentSetT<_component_t>& set = EnsureSet<_component_t>();
			set.emplace(entity, args...);
		}

		template<
			typename _component_t>
		void DestroyComponent(
				Entity entity)
		{
			ComponentSet* set = GetSet<_component_t>();
			if (set) {
				set->erase(entity);
			}
		}
	private:
		template<
			typename _component_t>
		ComponentSetT<_component_t>& EnsureSet() {
			using CSetType = ComponentSetT<_component_t>;

			ComponentId id = ComponentFamily::type<_component_t>;
			if (id == m_components.size()) {
				CSetType* set = new CSetType();
				m_components.push_back(set);
				return *set;
			}

			return *static_cast<CSetType*>(m_components.at(id));
		}

		template<
			typename _component_t>
		ComponentSet* GetSet() {
			using CSetType = ComponentSetT<_component_t>;

			ComponentId id = ComponentFamily::type<_component_t>;
			if (SetExists(id)) {
				return static_cast<CSetType*>(m_components.at(id));
			}

			return nullptr;
		}

		bool SetExists(
			ComponentId id)
		{
			return id < m_components.size();
		}
	};
}

namespace IwEntity {
	using ComponentId = unsigned int;
	using Entity      = unsigned int;
	using Archetype   = unsigned long long;

	class Space {
	private:
		using EntityMap = iwu::sparse_set<Entity, Archetype>;

		template<
			typename _component_t>
			using ComponentSet = iwu::sparse_set<Entity, _component_t>;

		using ComponentMap = iwu::sparse_set<ComponentId,
			iwu::sparse_set<Entity>*>;

		using ComponentGroup = iwu::family<Space>;

		EntityMap    m_entities;
		ComponentMap m_components;

	public:
		~Space() {
			for (auto set : m_components) {
				delete set;
			}
		}

		Entity CreateEntity() {
			static Entity nextEntity = 0;
			m_entities.emplace(++nextEntity, Archetype());

			return nextEntity;
		}

		void DestroyEntity(
			Entity entity)
		{
			if (m_entities.contains(entity)) {
				m_entities.erase(entity);
				for (auto set : m_components) {
					if (set->contains(entity)) {
						set->erase(entity);
					}
				}
			}
		}

		template<
			typename _component_t,
			typename... _args_t>
			void CreateComponent(
				Entity entity,
				const _args_t& ... args)
		{
			AddComponentToArchetype<_component_t>(entity);
			ComponentSet<_component_t>& set = EnsureSet<_component_t>();
			set.emplace(entity, _component_t{ args... });
		}

		template<
			typename _component_t>
			void DestroyComponent(
				Entity entity)
		{
			RemoveComponentFromArchetype<_component_t>(entity);
			ComponentSet<_component_t>& set = EnsureSet<_component_t>();
			if (set.contains(entity)) {
				set.erase(entity);
			}
		}

		template<
			typename _component_t>
			_component_t& GetComponent(
				Entity entity)
		{
			ComponentSet<_component_t>& set = EnsureSet<_component_t>();
			return set.at(entity);
		}
	private:
		template<
			typename _component_t>
			ComponentSet<_component_t>& EnsureSet() {
			unsigned int id = ComponentGroup::type<_component_t>;
			ComponentSet<_component_t>* set = nullptr;
			if (m_components.contains(id)) {
				set = static_cast<ComponentSet<_component_t>*>(
					m_components.at(id));
			}

			else {
				set = new iwu::sparse_set<Entity, _component_t>;
				m_components.emplace(id, set);
			}

			return *set;
		}

		template<
			typename _component_t>
			void AddComponentToArchetype(
				Entity entity)
		{
			unsigned int id = ComponentGroup::type<_component_t>;
			m_entities.at(entity) |= 1UL << id;
		}

		template<
			typename _component_t>
			void RemoveComponentFromArchetype(
				Entity entity)
		{
			unsigned int id = ComponentGroup::type<_component_t>;
			m_entities.at(entity) &= ~(1UL << id);
		}
	};
}
