#pragma once

#include "IwEntity.h"
#include "Component.h"
#include <memory>

namespace IwEntity {
	struct ArchetypeLayout {
		iwu::ref<const Component> Component;
		size_t Offset;
		size_t Onset;
	};

	struct Archetype2 {
		size_t Hash;
		size_t Size;
		size_t Count;
		ArchetypeLayout Layout[];
	};

	struct ArchetypeQuery {
		size_t Count;
		size_t Hashes[];
	};
}
