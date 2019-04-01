#pragma once

#include "iw/util/set/sparse_set.h"
#include "iw/util/type/family.h"
#include <assert.h>
#include <vector>
#include <tuple>
#include <bitset>
#include <cmath>

namespace IwEntity {
	using Entity      = unsigned int;
	using ComponentId = unsigned int;
	using Archetype   = unsigned int;

	class Space {
	private:
		using ComponentFamily = iwu::family<Space>;
		using ComponentSet    = iwu::sparse_set<ComponentId>;

		template<
			typename _c>
		using ComponentSetT = iwu::sparse_set<ComponentId, _c>;

		struct EntityData {
			Archetype Archetype;
			std::size_t Count;
		};

		struct EntityVector {
			std::vector<EntityData> Entities;

			bool operator()(
				int a,
				int b) const
			{
				int count = Entities[a].Count - Entities[b].Count;
				if (count == 0) {
					int size = Entities[a].Archetype
						- Entities[b].Archetype;

					if (size == 0) {
						return a < b;
					}

					return size >= 0;
				}

				return count >= 0;
			}
		};

		std::vector<ComponentSet*> m_components;
		EntityVector m_entities;

	public:
		Entity CreateEntity() {
			static Entity next = Entity();

			m_entities.Entities.push_back({ 0, 0 });

			return next++;
		}

		template<
			typename _c,
			typename... _args_t>
		void CreateComponent(
			Entity entity,
			const _args_t&... args)
		{
			ComponentSetT<_c>& set = EnsureSet<_c>();

			m_entities.Entities[entity].Archetype |= GetArchetype<_c>();

			set.emplace(entity, args...);
		}

		template<
			typename _c>
		void DestroyComponent(
				Entity entity)
		{
			ComponentSet* set = GetSet<_c>();
			if (set) {
				m_entities.Entities[entity].Archetype
					&= ~(GetArchetype<_c>());

				set->erase(entity);
			}
		}

		template<
			typename... _components_t>
		std::tuple<typename ComponentSetT<_components_t>::iterator...>
		GetComponents()
		{
			Archetype archetype = GetArchetype<_components_t...>();
			return std::make_tuple(GetSetItr<_components_t>(archetype)...);
		}

		void Sort() {
			for (auto& set : m_components) {
				set->sort(m_entities);
			}
		}
	private:
		template<
			typename _c>
		ComponentSetT<_c>& EnsureSet() {
			using CSetType = ComponentSetT<_c>;

			ComponentId id = ComponentFamily::type<_c>;
			if (id >= m_components.size()) {
				m_components.resize(id + 1);
			}

			if (!m_components.at(id)) {
				CSetType* set = new CSetType();
				m_components.at(id) = set;
				return *set;
			}

			return *static_cast<CSetType*>(m_components.at(id));
		}

		template<
			typename _c>
		ComponentSetT<_c>* GetSet() {
			using CSetType = ComponentSetT<_c>;

			ComponentId id = ComponentFamily::type<_c>;
			if (SetExists(id)) {
				return static_cast<CSetType*>(m_components.at(id));
			}

			return nullptr;
		}

		template<
			typename _c>
		typename ComponentSetT<_c>::iterator GetSetItr(
			Archetype archetype)
		{
			ComponentSetT<_c>* set = GetSet<_c>();
			assert(set != nullptr);

			auto itr = set->begin();
			auto end = set->end();
			while (itr != end) {
				EntityData ed = m_entities.Entities[set->map(itr.index())];
				ed.Archetype &= archetype;
				if (ed.Archetype == archetype) {
					return itr;
				}

				itr++;
			}

			return end;
		}

		bool SetExists(
			ComponentId id)
		{
			return id < m_components.size();
		}

		template<
			typename... _cs>
		Archetype GetArchetype() {
			return ((1 << ComponentFamily::type<_cs>) | ...);
		}
	};
}
