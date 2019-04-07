#pragma once

#include "IwEntity.h"
#include "View.h"
#include "iw/util/set/sparse_set.h"
#include "iw/util/type/family.h"
#include <assert.h>
#include <vector>
#include <tuple>
#include <bitset>
#include <cmath>

namespace IwEntity {
	class IWENTITY_API Space {
	private:
		using ComponentFamily = iwu::family<Space>;
		using ComponentSet    = iwu::sparse_set<Entity>;

		template<typename _c>
		using ComponentSetT = iwu::sparse_set<Entity, _c>;

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
		_c& CreateComponent(
			Entity entity,
			const _args_t&... args)
		{
			ComponentSetT<_c>& set = EnsureSet<_c>();

			m_entities.Entities[entity].Archetype |= GetArchetype<_c>();

			set.emplace(entity, args...);

			return set.at(entity);
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
		View<_components_t...> ViewComponents() {
			Archetype archetype = GetArchetype<_components_t...>();
			return View<_components_t...>(
				GetSetBegin<_components_t>(archetype)...,
				GetSetEnd  <_components_t>(archetype)...);
		}

		void Sort() {
			for (auto& set : m_components) {
				set->sort(m_entities);
			}
		}

		//temp
		void Log() {
			LOG_DEBUG << "Space";
			int i = 1;
			for (auto& set : m_components) {
				LOG_DEBUG << "";
				LOG_DEBUG << " Set     " << std::bitset<32>(i);
				i *= 2;

				for (auto& e : *set) {
					switch ((int)abs(log10(e + 0.9)) + 1) {
					case 1: LOG_DEBUG << "  " << e << "   :  " << std::bitset<32>(m_entities.Entities[e].Archetype); break;
					case 2: LOG_DEBUG << "  " << e << "  :  " << std::bitset<32>(m_entities.Entities[e].Archetype); break;
					case 3: LOG_DEBUG << "  " << e << " :  " << std::bitset<32>(m_entities.Entities[e].Archetype); break;
					}
				}
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
		typename ComponentSetT<_c>::iterator GetSetBegin(
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

		template<
			typename _c>
		typename ComponentSetT<_c>::iterator GetSetEnd(
			Archetype archetype)
		{
			ComponentSetT<_c>* set = GetSet<_c>();
			assert(set != nullptr);

			auto itr = set->end() - 1;
			auto begin = set->begin();
			while (itr != begin) {
				EntityData ed = m_entities.Entities[set->map(itr.index())];
				ed.Archetype &= archetype;
				if (ed.Archetype == archetype) {
					return itr + 1;
				}

				itr--;
			}

			return begin;
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
