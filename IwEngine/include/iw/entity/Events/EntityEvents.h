#pragma once

#include "iw/common/Events/EventGroups.h"
#include "iw/entity/Entity.h"

namespace iw {
namespace ECS {
	enum class EntityEventType
		: short
	{
		Destroy,
		Destroyed // need a component added and removed to clean up references to pointers etc.
	};

	struct EntityEvent
		: event
	{
		EntityHandle Entity;

		EntityEvent(
			EntityEventType type,
			EntityHandle entity)
			: event(val(EventGroup::ENTITY), val(type))
			, Entity(entity)
		{}
	};

	struct EntityDestroyEvent
		: EntityEvent
	{
		EntityDestroyEvent(
			EntityHandle entity)
			: EntityEvent(EntityEventType::Destroy, entity)
		{}
	};

	struct EntityDestroyedEvent
		: EntityEvent
	{
		EntityDestroyedEvent(
			EntityHandle entity)
			: EntityEvent(EntityEventType::Destroyed, entity)
		{}
	};
}

	using namespace ECS;
}
