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
		size_t hash = Component::Hash(components);
		iw::ref<Archetype>& archetype = m_hashed[hash];
		if (!archetype) {
			size_t bufSize = sizeof(Archetype)
				+ sizeof(ArchetypeLayout)
				* components.size();

			archetype = m_pool.alloc_ref_t<Archetype>(bufSize);

			size_t totalSize = 0;
			for (auto& component : components) {
				totalSize += component->Size;
			}

			archetype->Hash = hash;
			archetype->Count = components.size();
			archetype->Size = totalSize;

			size_t i    = 0;
			size_t size = 0;
			for (auto& component : components) {
				archetype->Layout[i].Component = component;
				archetype->Layout[i].Offset = size;

				size += component->Size;

				archetype->Layout[i].Onset = totalSize - size;

				i++;
			}

			archetype = m_archetypes.emplace_back(archetype);
		}

		return archetype;
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
}
