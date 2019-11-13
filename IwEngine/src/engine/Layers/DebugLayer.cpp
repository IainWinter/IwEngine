#include "iw/engine/Layers/DebugLayer.h"
#include "iw/engine/Components/Log.h"
#include "iw/data/Components/Transform.h"
#include "iw/engine/Time.h"
#include "imgui/imgui.h"
#include <gl\glew.h>

namespace IwEngine {
	DebugLayer::DebugLayer(
		IwEntity::Space& space,
		IW::Renderer& renderer,
		IW::AssetManager& asset)
		: Layer(space, renderer, asset, "Debug")
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

	void DebugLayer::ImGui() {
		float time = IwEngine::Time::FixedTime();
		ImGui::SliderFloat("Fixed timestep", &time, 0, 1);

		if (time != IwEngine::Time::FixedTime()) {
			IwEngine::Time::SetFixedTime(time);
		}
	}
}
