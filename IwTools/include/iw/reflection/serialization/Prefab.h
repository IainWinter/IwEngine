#pragma once

#include "Type.h"

namespace iw {
namespace Reflect {
	struct Prefab {
		std::vector<>
		iw::ref<Archetype> m_archetype;
		void* m_componentData;
		size_t m_componentCount;
	};
}
	using namespace ECS;
}
