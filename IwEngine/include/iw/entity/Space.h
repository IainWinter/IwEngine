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
		using ComponentFamily = iwu::family<Space>;

		std::vector<IComponentData*> m_components;
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
			_args_t&&... args)
		{
			Archetype oldArchetype = m_entities.Entities.at(entity).Archetype;
			Archetype& archetype   = m_entities.AssignComponent<_c>(entity);

			std::size_t index = 0;
			for (Archetype i = 1; i <= oldArchetype; i = i << 1, index++) {
				if ((oldArchetype & i) == i) { //pretty sure set has to exist for archetype to exist
					m_components.at(index)->UpdateChunk(entity, archetype);
				}
			}

			return EnsureComponentData<_c>()
				.CreateComponent(
					entity,
					archetype,
					std::forward<_args_t>(args)...);
		}

		//template<
		//	typename _c>
		//void DestroyComponent(
		//	Entity entity)
		//{
		//	ComponentData* components = GetComponentSet<_c>();
		//	if (components) {
		//		m_entities.Entities[entity].Archetype
		//			&= ~(GetArchetype<_c>());

		//		components->Components->erase(entity);
		//	}
		//}

		//template<
		//	typename... _cs>
		//View<_cs...> ViewComponents() {
		//	Archetype archetype = GetArchetype<_cs...>();
		//	return View<_cs...>(
		//		GetSetBegin<_cs>(archetype)...,
		//		GetSetEnd  <_cs>(archetype)...);
		//}

		void Sort() {
			//for (auto& set : m_components) {
			//	set.Components->sort(m_entities);
			//}
		}

		//temp
		void Log() {
			//LOG_DEBUG << "Space";
			//int i = 1;
			//for (auto& set : m_components) {
			//	LOG_DEBUG << " Set     " << std::bitset<32>(i);
			//	i *= 2;

			//	for (auto& e : *set.Components) {
			//		switch ((int)abs(log10(e + 0.9)) + 1) {
			//		case 1: LOG_DEBUG << "  " << e << "   :  " << std::bitset<32>(m_entities.Entities[e].Archetype); break;
			//		case 2: LOG_DEBUG << "  " << e << "  :  " << std::bitset<32>(m_entities.Entities[e].Archetype); break;
			//		case 3: LOG_DEBUG << "  " << e << " :  " << std::bitset<32>(m_entities.Entities[e].Archetype); break;
			//		}
			//	}

			//	LOG_DEBUG << "";
			//}
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

		//template<
		//	typename _c>
		//typename iwu::sparse_set<Entity, _c>::iterator GetSetBegin(
		//	Archetype archetype)
		//{
		//	ComponentData* components = GetComponentSet<_c>();
		//	ComponentSet<_c>& set = components->SetAs<_c>();

		//	assert(components != nullptr, "Component set does not exist!");

		//	auto itr = set.begin();
		//	auto end = set.end();
		//	while (itr != end) {
		//		EntityData ed = m_entities.Entities[set.map(itr.index())];
		//		ed.Archetype &= archetype;
		//		if (ed.Archetype == archetype) {
		//			return itr;
		//		}

		//		itr++;
		//	}

		//	return end;
		//}

		//template<
		//	typename _c>
		//typename iwu::sparse_set<Entity, _c>::iterator GetSetEnd(
		//	Archetype archetype)
		//{
		//	ComponentData* components = GetComponentSet<_c>();
		//	ComponentSet<_c>& set = components->SetAs<_c>();

		//	assert(components != nullptr, "Component set does not exist!");

		//	auto itr = set.end() - 1;
		//	auto begin = set.begin();
		//	while (itr != begin) {
		//		EntityData ed = m_entities.Entities[set.map(itr.index())];
		//		ed.Archetype &= archetype;
		//		if (ed.Archetype == archetype) {
		//			return itr + 1;
		//		}

		//		itr--;
		//	}

		//	return begin;
		//}

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
