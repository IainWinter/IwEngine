#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"

namespace iw {
namespace Engine {
	class EntityCleanupSystem
		: public System<>
	{
	public:
		IWENGINE_API
		EntityCleanupSystem();

		IWENGINE_API
		bool On(
			EntityDestroyedEvent& e) override;
	};
}

	using namespace Engine;
}
