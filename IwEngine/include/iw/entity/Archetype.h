#pragma once

#include "IwEntity.h"
#include "Component.h"
#include <memory>

namespace IwEntity {
	struct ArchetypeQuery {
		size_t Count;
		size_t Size;
		const std::weak_ptr<Component> Components[];
	};

	struct ArchetypeLayout {
		std::weak_ptr<Component> Component;
		size_t Offset;
		size_t Onset;
	};

	struct Archetype {
		size_t Size;
		size_t Count;
		const ArchetypeLayout Layout[];
	};

	struct EntityArchetype {
		std::weak_ptr<Archetype> Archetype;
	};

	//static ArchetypeQuery* MakeArchetypeQuery(
	//	std::initializer_list<ComponentType> components)
	//{
	//	size_t size = sizeof(ArchetypeQuery)
	//		+ sizeof(ComponentType)
	//		* components.size();

	//	ArchetypeQuery* query = (ArchetypeQuery*)malloc(size);
	//	if (query) {
	//		query->Count = components.size();

	//		auto itr = components.begin();
	//		for (size_t i = 0; i < query->Count; i++, itr++) {
	//			query->Components[0] = *itr;
	//		}
	//	}

	//	return query;
	//}
}
