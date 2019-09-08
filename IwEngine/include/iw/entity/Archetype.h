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

	struct Archetype2 {
		size_t Hash;
		size_t Size;
		size_t Count;
		ArchetypeLayout Layout[];
	};

	struct EntityArchetype {
		std::weak_ptr<Archetype2> Archetype;

		Archetype2& Get() {
			return *Archetype.lock();
		}
	};
}
