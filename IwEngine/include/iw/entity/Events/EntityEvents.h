#pragma once

#include "iw/events/event.h"
#include "iw/common/Events/EventGroups.h"
#include "iw/entity/EntityHandle.h"
#include "iw/entity/Archetype.h"

namespace iw {
namespace ECS {
	enum class EntityEventType
		: short
	{
		Created,
		Destroy,
		Destroyed,
		Moved
	};

	struct REFLECT EntityEvent
		: event
	{
		EntityHandle Entity;

		EntityEvent(
			EntityEventType type,
			EntityHandle entity
		)
			: event(val(EventGroup::ENTITY), val(type))
			, Entity(entity)
		{}
	};

	struct REFLECT EntityCreatedEvent
		: EntityEvent
	{
		EntityCreatedEvent(
			EntityHandle entity
		)
			: EntityEvent(EntityEventType::Created, entity)
		{}
	};

	struct REFLECT EntityDestroyEvent
		: EntityEvent
	{
		EntityDestroyEvent(
			EntityHandle entity
		)
			: EntityEvent(EntityEventType::Destroy, entity)
		{}
	};

	struct REFLECT EntityDestroyedEvent
		: EntityEvent
	{
		EntityDestroyedEvent(
			EntityHandle entity
		)
			: EntityEvent(EntityEventType::Destroyed, entity)
		{}
	};

	struct REFLECT EntityMovedEvent
		: EntityEvent
	{
		NO_REFLECT Archetype OldArchetype;
		NO_REFLECT Archetype NewArchetype;

		EntityMovedEvent(
			EntityHandle entity,
			Archetype oldArchetype,
			Archetype newArchetype
		)
			: EntityEvent(EntityEventType::Moved, entity)
			, OldArchetype(oldArchetype)
			, NewArchetype(newArchetype)
		{}
	};
}

	using namespace ECS;
}
