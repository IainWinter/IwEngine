#pragma once

#include "IwEntity.h"
#include "Component.h"
#include <memory>

namespace IW {
namespace ECS {
	struct ArchetypeLayout {
		iw::ref<Component> Component;
		size_t Offset;
		size_t Onset;
	};

	struct Archetype {
		size_t Hash;
		size_t Size;
		size_t Count;
		ArchetypeLayout Layout[];

		ArchetypeLayout* GetLayout(
			const iw::ref<Component>& component)
		{
			for (size_t i = 0; i < Count; i++) {
				if (component->Type == Layout[i].Component->Type) {
					return &Layout[i];
				}
			}

			return nullptr;
		}

		bool HasComponent(
			const iw::ref<Component>& component)
		{
			return GetLayout(component) != nullptr;
		}
	};

	struct ArchetypeQuery {
		size_t Count;
		size_t Hashes[];
	};
}

	using namespace ECS;
}
