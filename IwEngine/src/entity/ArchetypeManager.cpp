#include "iw/entity/ArchetypeManager.h"
#include <assert.h>

namespace IwEntity {
	iwu::ref<Archetype2> ArchetypeManager::CreateArchetype(
		std::initializer_list<iwu::ref<const Component>> components)
	{
		size_t hash = Component::Hash(components);
		auto& archetype = m_hashed[hash];
		if (!archetype) {
			size_t bufSize = sizeof(Archetype2)
				+ sizeof(ArchetypeLayout)
				* components.size();

			Archetype2* buf = (Archetype2*)malloc(bufSize); //todo: add custom memory allocation
			assert(buf);
			memset(buf, 0, bufSize);

			archetype = iwu::ref<Archetype2>(buf, free);
			m_archetypes.push_back(archetype);

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
		}

		return archetype;
	}

	iwu::ref<ArchetypeQuery> ArchetypeManager::MakeQuery(
		const ComponentQuery& query)
	{
		std::vector<size_t> matches;
		for (auto& archetype : m_archetypes) {
			bool match = false;
			for (size_t i = 0; i < query.All.size(); i++) {
				bool hasComponent = false;
				for (size_t j = 0; j < archetype->Count; j++) {
					if (query.All[i] == archetype->Layout[j].Component) {
						hasComponent = true;
						break;
					}
				}

				match = hasComponent;

				if (!hasComponent) {
					break;
				}
			}

			if (query.Any.size() > 0) {
				bool hasComponent = false;
				for (size_t i = 0; i < query.Any.size(); i++) {
					for (size_t j = 0; j < archetype->Count; j++) {
						if (query.Any[j] == archetype->Layout[i].Component) {
							hasComponent = true;
							break;
						}
					}

					if (hasComponent) {
						break;
					}
				}

				match = hasComponent;
			}

			if (query.None.size() > 0) {
				for (size_t i = 0; i < query.None.size(); i++) {
					for (size_t j = 0; j < archetype->Count; j++) {
						if (query.Any[j] == archetype->Layout[i].Component) {
							match = false;
							break;
						}
					}

					if (!match) {
						break;
					}
				}
			}

			if (match) {
				matches.push_back(archetype->Hash);
			}
		}

		size_t bufSize = sizeof(ArchetypeQuery)
			+ sizeof(size_t)
			* matches.size();

		ArchetypeQuery* buf = (ArchetypeQuery*)malloc(bufSize);
		assert(buf);
		memset(buf, 0, bufSize);

		buf->Count = matches.size();
		for (size_t i = 0; i < buf->Count; i++) {
			buf->Hashes[i] = matches[i];
		}

		return iwu::ref<ArchetypeQuery>(buf, free);
	}
}
