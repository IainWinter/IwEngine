#pragma once

#include "IwEntity.h"
#include "Archetype.h"

namespace iw {
namespace ECS {
	struct Prefab {
		iw::ref<Archetype> m_archetype;
		void* m_componentData;
		size_t m_componentCount;
	};
}
	using namespace ECS;
}
