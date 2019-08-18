#pragma once

#include "iw/entity2/IwEntity.h"
#include <vector>
#include <initializer_list>
#include <stdlib.h>

namespace IwEntity2 {
	struct ComponentTypeInfo {
		ComponentType Type;
		size_t        Size;
		size_t        Offset;
	};

	// Todo: Make this an struct with an array and more info
	struct IWENTITY2_API Archetype {
		size_t Count;
		size_t Size;

		ComponentTypeInfo* Layout;

		char Bitset[];

		const static size_t ArchetypeHeaderSize;

		ComponentTypeInfo GetTypeInfo(
			ComponentType type) const;

		bool EqualWith(
			Archetype& other,
			ComponentType type) const;

		bool EqualWithout(
			Archetype& other,
			ComponentType type) const;

		bool HasComponent(
			ComponentType type) const;

		size_t SizeBefore(
			ComponentType type) const;

		size_t SizeAfter(
			ComponentType type) const;

		inline bool Empty() const {
			return Count == 0;
		}

		static Archetype* MakeArchetype(
			std::initializer_list<ComponentTypeInfo> layout);
	};
}
