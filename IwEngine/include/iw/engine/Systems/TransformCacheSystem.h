#pragma once

#include "iw/engine/System.h"
#include "iw/engine/Core.h"
#include "iw/common/Components/Transform.h"
#include "iw/physics/Collision/Collider.h"

namespace iw {
namespace Engine {
	struct TransformCacheSystem : iw::SystemBase
	{
		TransformCacheSystem()
			: iw::SystemBase("Transform cache")
		{}

		IWENGINE_API void Update();
		IWENGINE_API void FixedUpdate();
	};
}

	using namespace Engine;
}
