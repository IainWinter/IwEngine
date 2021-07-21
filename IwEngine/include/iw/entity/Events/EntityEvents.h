#pragma once

#include "iw/common/Events/EventGroups.h"
#include "iw/entity/Entity.h"

namespace iw {
namespace ECS {
	enum class EntityEventType
		: short
	{
		Destroy,
		Destroyed,
		Moved
	};

	struct EntityEvent
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

	struct EntityDestroyEvent
		: EntityEvent
	{
		EntityDestroyEvent(
			EntityHandle entity
		)
			: EntityEvent(EntityEventType::Destroy, entity)
		{}
	};

	struct EntityDestroyedEvent
		: EntityEvent
	{
		EntityDestroyedEvent(
			EntityHandle entity
		)
			: EntityEvent(EntityEventType::Destroyed, entity)
		{}
	};

	struct EntityMovedEvent
		: EntityEvent
	{
		ref<Archetype> OldArchetype;
		ref<Archetype> NewArchetype;

		EntityMovedEvent(
			EntityHandle entity,
			ref<Archetype> oldArchetype,
			ref<Archetype> newArchetype
		)
			: EntityEvent(EntityEventType::Moved, entity)
			, OldArchetype(oldArchetype)
			, NewArchetype(newArchetype)
		{}
	};
}

	using namespace ECS;
}
