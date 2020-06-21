#pragma once

#include "IwEntity.h"

namespace iw {
namespace ECS {
	struct EntityHandle {
		IWENTITY_API
		static const EntityHandle Empty;

		size_t Index;
		short  Version;
		bool   Alive;

		bool operator==(
			const EntityHandle& other) const
		{
			return  Index   == other.Index
				&& Version == other.Version;
		}

		bool operator!=(
			const EntityHandle& other) const
		{
			return !operator==(other);
		}
	};
}

	using namespace ECS;
}
