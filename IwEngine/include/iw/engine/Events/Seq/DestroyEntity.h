#pragma once
#include "iw/events/seq/event_seq.h"
#include "iw/entity/Entity.h"
#include "iw/common/Components/Transform.h"

namespace iw {
namespace Engine {
	struct DestroyEntity:event_seq_item {
		Entity& entity;

		DestroyEntity(
			Entity& entity)
			: entity(entity)
		{}

		bool update() override {
			entity.Find<Transform>()->SetParent(nullptr);
			entity.Space->DestroyEntity(entity.Index());
			return true;
		}
	};
}

	using namespace Engine;
}
