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

		struct ComponentChunk {
			Archetype Archetype;
			std::size_t Begin;
			std::size_t End;
		};

		template<
			typename _c>
		using Set = iwu::sparse_set<Entity, _c>;

		struct ComponentSet {
			iwu::sparse_set<Entity>* Components;
			std::vector<ComponentChunk> Chunks;

			template<
				typename _c>
			Set<_c>* As() {
				return static_cast<Set<_c>*>(Components);
			}
		};

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

		std::vector<ComponentSet> m_components;
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
			ComponentSet& components = EnsureComponentSet<_c>();
			Set<_c>* set = components.As<_c>();

			m_entities.Entities[entity].Archetype |= GetArchetype<_c>();

			set->emplace(entity, args...);

			return set->at(entity);
		}

		template<
			typename _c>
		void DestroyComponent(
			Entity entity)
		{
			ComponentSet* components = GetComponentSet<_c>();
			if (components) {
				m_entities.Entities[entity].Archetype
					&= ~(GetArchetype<_c>());

				components->Components->erase(entity);
			}
		}

		template<
			typename... _cs>
		View<_cs...> ViewComponents() {
			Archetype archetype = GetArchetype<_cs...>();
			return View<_cs...>(
				GetSetBegin<_cs>(archetype)...,
				GetSetEnd  <_cs>(archetype)...);
		}

		void Sort() {
			for (auto& set : m_components) {
				set.Components->sort(m_entities);
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

				for (auto& e : *set.Components) {
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
		ComponentSet& EnsureComponentSet() {
			ComponentId id = ComponentFamily::type<_c>;
			if (id >= m_components.size()) {
				m_components.resize(id + 1);
			}

			ComponentSet& components = m_components.at(id);
			if (!components.Components) {
				Set<_c>* set = new Set<_c>();
				components.Components = set;
			}

			return components;
		}

		template<
			typename _c>
		ComponentSet* GetComponentSet() {
			ComponentId id = ComponentFamily::type<_c>;
			if (SetExists(id)) {
				return static_cast<Set<_c>*>(m_components.at(id).Components);
			}

			return nullptr;
		}

		template<
			typename _c>
		typename iwu::sparse_set<Entity, _c>::iterator GetSetBegin(
			Archetype archetype)
		{
			ComponentSet* components = GetComponentSet<_c>();
			Set<_c>* set = components->As<_c>();

			assert(components != nullptr, "Component set does not exist!");

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
		typename iwu::sparse_set<Entity, _c>::iterator GetSetEnd(
			Archetype archetype)
		{
			ComponentSet* components = GetComponentSet<_c>();
			Set<_c>* set = components->As<_c>();

			assert(components != nullptr, "Component set does not exist!");

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
