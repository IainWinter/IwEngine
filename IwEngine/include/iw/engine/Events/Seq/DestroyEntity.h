#pragma once

#include "EventTask.h"
#include "iw/entity/Entity.h"
#include "iw/common/Components/Transform.h"
#include "iw/entity/Events/EntityEvents.h"

namespace iw {
namespace Engine {
	struct DestroyEntity
		: EventTask
	{
	private:
		Entity entity;
		bool async;

	public:
		DestroyEntity(
			Entity entity,
			bool async = false)
			: entity(entity)
			, async(async)
		{}

		bool update() override {
			if (!entity) return true;

			if (async) {
				Bus->push<EntityDestroyEvent>(entity);
			}

			else {
				entity.Find<Transform>()->SetParent(nullptr);
				entity.Destroy();
			}

			return true;
		}
	};
}

	using namespace Engine;
}
