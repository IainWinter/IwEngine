#include "iw/engine/Systems/LightShadowRenderSystem.h"

namespace iw {
namespace Engine {
	LightShadowRenderSystem::LightShadowRenderSystem()
		: System<Transform, Light>("Physics")
	{}

	void LightShadowRenderSystem::Update(
		EntityComponentArray& eca)
	{
		for (auto entity : eca) {
			auto [transform, light] = entity.Components.Tie<Components>();

		}
	}
}
}
