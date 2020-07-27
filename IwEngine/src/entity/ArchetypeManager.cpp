#include "iw/entity/ArchetypeManager.h"
#include "iw/events/callback.h"
#include <assert.h>

#include <algorithm>

namespace iw {
namespace ECS {
	ArchetypeManager::ArchetypeManager()
		: m_pool(1024)
	{}

	iw::ref<Archetype>& ArchetypeManager::CreateArchetype(
		std::initializer_list<iw::ref<Component>> components)
	{
		return CreateArchetype(components.begin(), components.end());
	}

	iw::ref<Archetype>& ArchetypeManager::CreateArchetype(
		const iw::ref<Component>* begin,
		const iw::ref<Component>* end)
	{
		size_t hash = Component::Hash(begin, end);
		iw::ref<Archetype>& archetype = m_hashed[hash];
		if (!archetype) {
			size_t bufSize = sizeof(Archetype)
				+ sizeof(ArchetypeLayout)
				* std::distance(begin, end);

			archetype = m_pool.alloc_ref<Archetype>(bufSize);

			size_t totalSize = 0;
			for (auto itr = begin; itr != end; itr++) {
				totalSize += (*itr)->Size;
			}

			archetype->Hash  = hash;
			archetype->Count = std::distance(begin, end);
			archetype->Size  = totalSize;

			std::vector<ref<Component>> components(begin, end);

			std::sort(components.begin(), components.end(), [](ref<Component>& a, ref<Component>& b) {
				return a->Id < b->Id;
			});

			for (size_t i = 0, size = 0; i < components.size(); i++) {
				ref<Component>& component = components[i];

				archetype->Layout[i].Component = component;
				archetype->Layout[i].Offset    = size;

				size += component->Size;

				archetype->Layout[i].Onset = totalSize - size;
			}

			archetype = m_archetypes.emplace_back(archetype);
		}

		return archetype;
	}

	iw::ref<Archetype>& ArchetypeManager::AddComponent(
		iw::ref<Archetype> archetype,
		iw::ref<Component> component)
	{
		size_t bufSize = sizeof(Archetype)
			+ sizeof(ArchetypeLayout)
			* archetype->Count + 1;

		std::vector<iw::ref<Component>> components;
		for (size_t i = 0; i < archetype->Count; i++) {
			components.push_back(archetype->Layout[i].Component);
		}

		components.push_back(component);

		return CreateArchetype(components.begin()._Ptr, components.end()._Ptr);
	}

	iw::ref<Archetype>& ArchetypeManager::RemoveComponent(
		iw::ref<Archetype> archetype,
		iw::ref<Component> component)
	{
		size_t bufSize = sizeof(Archetype)
			+ sizeof(ArchetypeLayout)
			* archetype->Count - 1;

		std::vector<iw::ref<Component>> components;
		for (size_t i = 0; i < archetype->Count; i++) {
			iw::ref<Component> c = archetype->Layout[i].Component;
			if(c != component) {
				components.push_back(c);	
			}
		}

		return CreateArchetype(components.begin()._Ptr, components.end()._Ptr);
	}

	iw::ref<ArchetypeQuery> ArchetypeManager::MakeQuery(
		const iw::ref<ComponentQuery>& query)
	{
		std::vector<size_t> matches;
		for (const iw::ref<Archetype>& archetype : m_archetypes) { // not sure whats the best search order is...												   
			bool match = true;

			// reject if archeptye has component in none list

			for (const iw::ref<Component>& component : query->GetNone()) {
				if (archetype->HasComponent(component)) {
					match = false;
					break;
				}
			}

			if (!match) {
				continue;
			}

			// reject if archetype doesn't have a component in all list

			for (const iw::ref<Component>& component : query->GetAll()) {
				if (!archetype->HasComponent(component)) {
					match = false;
					break;
				}
			}

			if (!match) {
				continue;
			}

			// reject if archetype doesn't have at lest one component in any list

			bool hasAComponent = false;
			for (const iw::ref<Component>& component : query->GetAll()) {
				if (archetype->HasComponent(component)) {				
					hasAComponent = true;
					break;
				}
			}

			match = hasAComponent;

			if (!match) {
				continue;
			}

			matches.push_back(archetype->Hash);
		}

		size_t bufSize = sizeof(ArchetypeQuery)
			+ sizeof(size_t)
			* matches.size();

		iw::ref<ArchetypeQuery> q = m_pool.alloc_ref<ArchetypeQuery>(bufSize);

		q->Count = matches.size();
		for (size_t i = 0; i < q->Count; i++) {
			q->Hashes[i] = matches[i];
		}

		return q;
	}

	void ArchetypeManager::Clear() {
		m_hashed.clear();
		m_archetypes.clear();
		m_pool.reset();
	}

	const std::vector<ref<Archetype>>& ArchetypeManager::Archetypes() const {
		return m_archetypes;
	}
}
}
