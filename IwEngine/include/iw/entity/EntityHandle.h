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

	struct ehandle_hash {
		size_t operator() (const EntityHandle& handle) const {
			return ((size_t)handle.Index   * 0x1f1f1f1f)
				^ ((size_t)handle.Version * 0x2b2b2b2b)
				^  (size_t)handle.Alive;
		}
	};
}

	using namespace ECS;
}
