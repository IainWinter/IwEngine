#pragma once

#include "iw/util/set/sparse_set.h"
#include "iw/util/type/family.h"
#include <assert.h>
#include <vector>
#include <tuple>
#include <bitset>
#include <cmath>

namespace IwEntity5 {
	using Entity      = unsigned int;
	using ComponentId = unsigned int;
	using Archetype   = std::bitset<32>;

	class Space {
		using ComponentFamily = iwu::family<Space>;
		using ComponentSet    = iwu::sparse_set<ComponentId>;

		template<
			typename _c>
		using ComponentSetT = iwu::sparse_set<ComponentId, _c>;

	private:
		std::vector<ComponentSet*> m_components;
		struct EntityVector {
			std::vector<Archetype> Entities;

			bool operator()(
				int a,
				int b) const
			{
				int count = Entities[a].count() - Entities[b].count();
				if (count == 0) {
					int size = Entities[a].to_ulong()
						- Entities[b].to_ulong();

					if (size == 0) {
						return a < b;
					}

					return size >= 0;
				}

				return count >= 0;
			}
		} m_entities;

	public:
		Entity CreateEntity() {
			static Entity next = Entity();

			m_entities.Entities.push_back(next);
			m_entities.Entities.back().reset();

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

			m_entities.Entities[entity].set(
				ComponentFamily::type<_c>, true);

			set.emplace(entity, args...);
		}

		template<
			typename _c>
		void DestroyComponent(
				Entity entity)
		{
			ComponentSet* set = GetSet<_c>();
			if (set) {
				m_entities.Entities[entity].set(
					ComponentFamily::type<_c>, false);

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
					case 1: LOG_DEBUG << "  " << e << "   :  " << m_entities.Entities[e]; break;
					case 2: LOG_DEBUG << "  " << e << "  :  " << m_entities.Entities[e];  break;
					case 3: LOG_DEBUG << "  " << e << " :  " << m_entities.Entities[e];   break;
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
			if (id == m_components.size()) {
				CSetType* set = new CSetType();
				m_components.push_back(set);
				return *set;
			}

			return *static_cast<CSetType*>(m_components.at(id));
		}

		template<
			typename _c>
		ComponentSetT<_c>* GetSet() {
			ComponentId id = ComponentFamily::type<_c>;
			if (SetExists(id)) {
				return static_cast<ComponentSetT<_c>*>(
					m_components.at(id));
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
				Archetype ea = m_entities.Entities[set->map(itr.index())];
				ea &= archetype;
				if (ea == archetype) {
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
			typename... _components_t>
		Archetype GetArchetype() {
			return ((1 << ComponentFamily::type<_components_t>) | ...);
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
			typename _c>
			using ComponentSet = iwu::sparse_set<Entity, _c>;

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
			typename _c,
			typename... _args_t>
			void CreateComponent(
				Entity entity,
				const _args_t& ... args)
		{
			AddComponentToArchetype<_c>(entity);
			ComponentSet<_c>& set = EnsureSet<_c>();
			set.emplace(entity, _c{ args... });
		}

		template<
			typename _c>
			void DestroyComponent(
				Entity entity)
		{
			RemoveComponentFromArchetype<_c>(entity);
			ComponentSet<_c>& set = EnsureSet<_c>();
			if (set.contains(entity)) {
				set.erase(entity);
			}
		}

		template<
			typename _c>
			_c& GetComponent(
				Entity entity)
		{
			ComponentSet<_c>& set = EnsureSet<_c>();
			return set.at(entity);
		}
	private:
		template<
			typename _c>
			ComponentSet<_c>& EnsureSet() {
			unsigned int id = ComponentGroup::type<_c>;
			ComponentSet<_c>* set = nullptr;
			if (m_components.contains(id)) {
				set = static_cast<ComponentSet<_c>*>(
					m_components.at(id));
			}

			else {
				set = new iwu::sparse_set<Entity, _c>;
				m_components.emplace(id, set);
			}

			return *set;
		}

		template<
			typename _c>
			void AddComponentToArchetype(
				Entity entity)
		{
			unsigned int id = ComponentGroup::type<_c>;
			m_entities.at(entity) |= 1UL << id;
		}

		template<
			typename _c>
			void RemoveComponentFromArchetype(
				Entity entity)
		{
			unsigned int id = ComponentGroup::type<_c>;
			m_entities.at(entity) &= ~(1UL << id);
		}
	};
}
