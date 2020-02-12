#include "iw/entity/ArchetypeManager.h"
#include "iw/events/callback.h"
#include <assert.h>

namespace IW {
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

			archetype = m_pool.alloc_ref_t<Archetype>(bufSize);

			size_t totalSize = 0;
			for (auto itr = begin; itr != end; itr++) {
				totalSize += (*itr)->Size;
			}

			archetype->Hash  = hash;
			archetype->Count = std::distance(begin, end);
			archetype->Size  = totalSize;

			size_t i = 0;
			size_t size = 0;
			for (auto itr = begin; itr != end; itr++) {
				archetype->Layout[i].Component = *itr;
				archetype->Layout[i].Offset    = size;

				size += (*itr)->Size;

				archetype->Layout[i].Onset = totalSize - size;

				i++;
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
		for (auto& archetype : m_archetypes) {
			bool match = false;
			for (size_t i = 0; i < query->Count; i++) {
				bool hasComponent = false;
				for (size_t j = 0; j < archetype->Count; j++) {
					if (query->Components[i] == archetype->Layout[j].Component) {
						hasComponent = true;
						break;
					}
				}

				match = hasComponent;

				if (!hasComponent) {
					break;
				}
			}

			//if (query.Any.size() > 0) {
			//	bool hasComponent = false;
			//	for (size_t i = 0; i < query.Any.size(); i++) {
			//		for (size_t j = 0; j < archetype->Count; j++) {
			//			if (query.Any[j] == archetype->Layout[i].Component) {
			//				hasComponent = true;
			//				break;
			//			}
			//		}
			//
			//		if (hasComponent) {
			//			break;
			//		}
			//	}
			//
			//	match = hasComponent;
			//}
			//
			//if (query.None.size() > 0) {
			//	for (size_t i = 0; i < query.None.size(); i++) {
			//		for (size_t j = 0; j < archetype->Count; j++) {
			//			if (query.Any[j] == archetype->Layout[i].Component) {
			//				match = false;
			//				break;
			//			}
			//		}
			//
			//		if (!match) {
			//			break;
			//		}
			//	}
			//}

			if (match) {
				matches.push_back(archetype->Hash);
			}
		}

		size_t bufSize = sizeof(ArchetypeQuery)
			+ sizeof(size_t)
			* matches.size();

		iw::ref<ArchetypeQuery> q = m_pool.alloc_ref_t<ArchetypeQuery>(bufSize);

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
}
