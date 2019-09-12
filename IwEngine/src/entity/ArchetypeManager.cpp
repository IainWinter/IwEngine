#include "iw/entity/ArchetypeManager.h"
#include <assert.h>

namespace IwEntity {
	iwu::ref<Archetype2> ArchetypeManager::CreateArchetype(
		std::initializer_list<iwu::ref<const Component>> components)
	{
		size_t hash = Component::Hash(components);
		auto& archetype = m_archetypes[hash];
		if (!archetype) {
			size_t bufSize = sizeof(Archetype2)
				+ sizeof(ArchetypeLayout)
				* components.size();

			Archetype2* buf = (Archetype2*)malloc(bufSize); //todo: add custom memory allocation
			assert(buf);
			memset(buf, 0, bufSize);

			archetype = iwu::ref<Archetype2>(buf, free);

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
}
