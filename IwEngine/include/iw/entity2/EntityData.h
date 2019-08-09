#pragma once

#include "iw/entity2/Archetype.h"

namespace IwEntity2 {
	struct EntityData {
		Entity       Id;
		Archetype    Arch;
		unsigned int Version;
		bool         Alive;

		//EntityData(
		//	Entity id,
		//	Archetype    archetype,
		//	unsigned int version,
		//	bool         alive)
		//	: Id(id)
		//	, Arch(archetype)
		//	, Version(version)
		//	, Alive(alive)
		//{}

		//EntityData(const EntityData&) = default;
		//EntityData(EntityData&&) = default;

		//EntityData& operator=(const EntityData&) = default;
		//EntityData& operator=(EntityData&&)      = default;
	};
}
