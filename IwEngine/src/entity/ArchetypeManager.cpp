#include "iw/entity/ArchetypeManager.h"
#include <assert.h>

namespace IwEntity {
	std::weak_ptr<Archetype2> ArchetypeManager::CreateArchetype(
		std::initializer_list<std::weak_ptr<Component>> components)
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

			archetype = std::shared_ptr<Archetype2>(buf);

			size_t totalSize = 0;
			for (std::weak_ptr<Component> component : components) {
				totalSize += component.lock()->Size;
			}

			archetype->Hash = hash;
			archetype->Count = components.size();
			archetype->Size = totalSize;

			size_t i = 0;
			size_t size = 0;
			for (std::weak_ptr<Component> component : components) {
				archetype->Layout[i].Component = component;
				archetype->Layout[i].Offset = size;

				size += component.lock()->Size;

				archetype->Layout[i].Onset = totalSize - size;

				i++;
			}
		}

		return archetype;
	}
}
