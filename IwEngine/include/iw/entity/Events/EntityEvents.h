#pragma once

#include "iw/common/Events/EventGroups.h"
#include "iw/entity/Entity.h"

namespace iw {
namespace ECS {
	enum class EntityEventType
		: short
	{
		Destroy,
		Destroyed
	};

	struct EntityEvent
		: event
	{
		Entity Entity;

		EntityEvent(
			EntityEventType type,
			iw::Entity entity)
			: event(val(EventGroup::ENTITY), val(type))
			, Entity(entity)
		{}
	};

	struct EntityDestroyEvent
		: EntityEvent
	{
		EntityDestroyEvent(
			iw::Entity entity)
			: EntityEvent(EntityEventType::Destroy, entity)
		{}
	};

	struct EntityDestroyedEvent
		: EntityEvent
	{
		EntityDestroyedEvent(
			iw::Entity entity)
			: EntityEvent(EntityEventType::Destroyed, entity)
		{}
	};
}

	using namespace ECS;
}
