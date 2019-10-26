#include "iw/engine/Layers/DebugLayer.h"
#include "iw/engine/Components/Log.h"
#include "iw/data/Components/Transform.h"
#include <gl\glew.h>

namespace IwEngine {
	DebugLayer::DebugLayer(
		IwEntity::Space& space,
		IW::Graphics::Renderer& renderer)
		: Layer(space, renderer, "Debug")
	{}

	struct Components {
		IW::Transform* Transform;
		VectorLog* Log;
	};

	void DebugLayer::PostUpdate() {
		//for (auto entity : Space.Query<IW::Transform, VectorLog>()) {
		//	auto [transform, log] = entity.Components.Tie<Components>();
		//}
	}
}
