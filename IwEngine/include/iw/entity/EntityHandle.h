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

		bool operator==(
			const EntityHandle& other)
		{
			return  Index == other.Index
				&& Version == other.Version;
		}

		bool operator!=(
			const EntityHandle& other)
		{
			return !operator==(other);
		}
	};
}

	using namespace ECS;
}
