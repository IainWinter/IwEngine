#pragma once

#include "IwEntity.h"
#include "Archetype.h"
#include "iw/util/memory/pool_allocator.h"
#include <unordered_map>
#include <typeindex>

namespace IwEntity {
	class ArchetypeManager {
	private:
		std::unordered_map<size_t, std::shared_ptr<Archetype>> m_archetypes;
		iwu::pool_allocator m_alloc; //tmp alloc

	public:
		EntityArchetype CreateArchetype(
			std::initializer_list<Component> components)
		{
			auto archetype = m_archetypes[Component::Hash(components)];
			if (!archetype) {
				size_t size = 0;
				for (Component component : components) {
					size += component.Size;
				}

				std::shared_ptr<Archetype> memory(m_alloc.alloc<Archetype>(), iwu::pool_allocator::free);


				archetype = std::make_shared<Archetype>();
				archetype->Count  = components.size();
				archetype->Size   = size;
				archetype->Layout = ;
			}

			return EntityArchetype{ archetype };
		}
	};
}
