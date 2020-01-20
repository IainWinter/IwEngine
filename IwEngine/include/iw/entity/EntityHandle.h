#pragma once

#include "IwEntity.h"

namespace IW {
namespace ECS {
	struct EntityHandle {
		IWENTITY_API
		static const EntityHandle Empty;

		size_t Index;
		short  Version;
		bool   Alive;
	};
}

	using namespace ECS;
}
