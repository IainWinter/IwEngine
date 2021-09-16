#pragma once

#include "IwEntity.h"
#include "Component.h"

namespace iw {
namespace ECS {
	struct ArchetypeLayout {
		ref<Component> Component;
		size_t Offset = 0;
		size_t Onset = 0;
	};

	struct Archetype {
		size_t Hash = 0;
		size_t Size = 0;
		size_t Count = 0;
		ref<ArchetypeLayout[]> Layout;

		const ArchetypeLayout* GetLayout(
			const ref<Component>& component) const
		{
			for (size_t i = 0; i < Count; i++) {
				if (component->Type == Layout[i].Component->Type) {
					return &Layout[i];
				}
			}

			return nullptr;
		}

		ArchetypeLayout* GetLayout(
			size_t index) const
		{
			return &Layout[index];
		}

		bool HasComponent(
			const ref<Component>& component) const
		{
			return GetLayout(component) != nullptr;
		}
	};

	struct ArchetypeQuery {
		size_t Count = 0;
		ref<size_t[]> Hashes;
	};
}

	using namespace ECS;
}
