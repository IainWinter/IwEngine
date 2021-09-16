#include "iw/entity/ArchetypeManager.h"
#include "iw/events/callback.h"
#include <assert.h>

#include <algorithm>

namespace iw {
namespace ECS {
	ArchetypeManager::ArchetypeManager()
		: m_pool(1024)
	{}

	Archetype ArchetypeManager::CreateArchetype(
		std::initializer_list<ref<Component>> components)
	{
		return CreateArchetype(components.begin(), components.end());
	}

	Archetype ArchetypeManager::CreateArchetype(
		const ref<Component>* begin,
		const ref<Component>* end)
	{
		size_t hash = Component::Hash(begin, end);
		
		if (m_hashed.find(hash) == m_hashed.end())
		{
			size_t totalSize = 0;
			for (auto itr = begin; itr != end; itr++) {
				totalSize += (*itr)->Size;
			}

			Archetype archetype;
			archetype.Hash = hash;
			archetype.Size = totalSize;
			archetype.Count = std::distance(begin, end);
			archetype.Layout = m_pool.alloc_ref_c<ArchetypeLayout>(archetype.Count);

			std::vector<ref<Component>> components(begin, end);

			std::sort(components.begin(), components.end(), [](ref<Component>& a, ref<Component>& b) {
				return a->Id < b->Id;
			});

			for (size_t i = 0, size = 0; i < components.size(); i++) {
				ref<Component>& component = components[i];

				archetype.Layout[i].Component = component;
				archetype.Layout[i].Offset    = size;

				size += component->Size;

				archetype.Layout[i].Onset = totalSize - size;
			}

			m_hashed[hash] = m_archetypes.size();
			m_archetypes.emplace_back(archetype);
		}

		return m_archetypes[m_hashed[hash]];
	}

	Archetype ArchetypeManager::AddComponent(
		const Archetype& archetype,
		ref<Component> component)
	{
		std::vector<ref<Component>> components;
		for (size_t i = 0; i < archetype.Count; i++) {
			components.push_back(archetype.Layout[i].Component);
		}

		components.push_back(component);

		return CreateArchetype(components.begin()._Ptr, components.end()._Ptr);
	}

	Archetype ArchetypeManager::RemoveComponent(
		const Archetype& archetype,
		ref<Component> component)
	{
		std::vector<ref<Component>> components;
		for (size_t i = 0; i < archetype.Count; i++) {
			ref<Component> c = archetype.Layout[i].Component;
			if(c != component) {
				components.push_back(c);	
			}
		}

		return CreateArchetype(components.begin()._Ptr, components.end()._Ptr);
	}

	ArchetypeQuery ArchetypeManager::MakeQuery(
		const ComponentQuery& query)
	{
		std::vector<size_t> matches;
		for (const Archetype& archetype : m_archetypes) { // not sure whats the best search order is...												   
			bool match = true;

			// reject if archeptye has component in none list

			for (const ref<Component>& component : query.GetNone()) {
				if (archetype.HasComponent(component)) {
					match = false;
					break;
				}
			}

			if (!match) {
				continue;
			}

			// reject if archetype doesn't have a component in all list

			for (const ref<Component>& component : query.GetAll()) {
				if (!archetype.HasComponent(component)) {
					match = false;
					break;
				}
			}

			if (!match) {
				continue;
			}

			// reject if archetype doesn't have at lest one component in any list

			bool hasAComponent = false;
			for (const ref<Component>& component : query.GetAll()) {
				if (archetype.HasComponent(component)) {
					hasAComponent = true;
					break;
				}
			}

			match = hasAComponent;

			if (!match) {
				continue;
			}

			matches.push_back(archetype.Hash);
		}

		ArchetypeQuery aquery;
		aquery.Count = matches.size();
		aquery.Hashes = m_pool.alloc_ref_c<size_t>(aquery.Count);
		
		for (size_t i = 0; i < aquery.Count; i++) {
			aquery.Hashes[i] = matches[i];
		}

		return aquery;
	}

	void ArchetypeManager::Clear() {
		m_hashed.clear();
		m_archetypes.clear();
		m_pool.reset();
	}

	const std::vector<Archetype>& ArchetypeManager::Archetypes() const {
		return m_archetypes;
	}
}
}
