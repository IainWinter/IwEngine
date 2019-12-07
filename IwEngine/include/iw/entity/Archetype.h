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
	};

	struct ArchetypeQuery {
		size_t Count;
		size_t Hashes[];
	};
}

	using namespace ECS;
}
