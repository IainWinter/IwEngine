#pragma once

#include "IwEntity.h"
#include "Component.h"
#include <memory>

namespace IwEntity {
	struct ArchetypeLayout {
		std::weak_ptr<Component> Component;
		size_t Offset;
		size_t Onset;
	};

	struct Archetype {
		size_t Size;
		size_t Count;
		const ArchetypeLayout Layout[];

		static Archetype* MakeArchetype(
			ArchetypeQuery* query)
		{
			size_t size = sizeof(Archetype)
				+ sizeof(ArchetypeLayout)
				* query->Count;

			Archetype* archetype = (Archetype*)malloc(size);
			archetype->Count = query->Count;

			for (size_t i = 0; i < archetype->Count; i++) {

			}
		}
	};

	struct ArchetypeQuery {
		size_t Count;
		const std::weak_ptr<Component> Components[];
	};
}
