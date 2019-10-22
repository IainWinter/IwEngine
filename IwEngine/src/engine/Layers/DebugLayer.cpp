#include "iw/engine/Layers/DebugLayer.h"
#include "iw/engine/Components/Log.h"
#include "iw/data/Components/Transform.h"
#include <gl\glew.h>

namespace IwEngine {
	DebugLayer::DebugLayer(
		IwEntity::Space& space,
		IwGraphics::RenderQueue& renderQueue)
		: Layer(space, renderQueue, "Debug")
	{}

	struct Components {
		Transform* Transform;
		VectorLog* Log;
	};

	void DebugLayer::PostUpdate() {
		//for (auto entity : Space.Query<Transform, VectorLog>()) {
		//	auto [transform, log] = entity.Components.Tie<Components>();
		//}
	}
}
