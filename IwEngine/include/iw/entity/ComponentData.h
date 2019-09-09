#pragma once

#include "IwEntity.h"
#include "Archetype.h"

namespace IwEntity {
	struct ComponentData {
		std::weak_ptr<Archetype2> Archetype;
		void* Components[];
	};
}
