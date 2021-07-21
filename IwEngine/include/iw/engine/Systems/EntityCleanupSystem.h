#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"

namespace iw {
namespace Engine {
	class EntityCleanupSystem
		: public SystemBase
	{
	public:
		IWENGINE_API
		EntityCleanupSystem();

		IWENGINE_API bool On(EntityDestroyEvent&   e) override;
		IWENGINE_API bool On(EntityDestroyedEvent& e) override;
		IWENGINE_API bool On(EntityMovedEvent&     e) override;
	};
}

	using namespace Engine;
}
