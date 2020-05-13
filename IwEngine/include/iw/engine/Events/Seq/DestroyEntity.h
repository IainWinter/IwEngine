#pragma once

#include "iw/events/seq/event_task.h"
#include "iw/common/Components/Transform.h"
#include "iw/entity/Entity.h"

namespace iw {
namespace Engine {
	struct DestroyEntity
		: event_task
	{
	private:
		Entity& entity;

	public:
		DestroyEntity(
			Entity& entity)
			: entity(entity)
		{}

		bool update() override {
			entity.Find<Transform>()->SetParent(nullptr);
			entity.Space->DestroyEntity(entity.Index()); // this should set alive to false but doesnt
			return true;
		}
	};
}

	using namespace Engine;
}
