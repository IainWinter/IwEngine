#include "iw/engine/Systems/ParticleUpdateSystem.h"

namespace iw {
namespace Engine {
	ParticleUpdateSystem::ParticleUpdateSystem()
		: System<ParticleSystem<StaticParticle>>("Particle Update")
	{}

	void ParticleUpdateSystem::Update(
		EntityComponentArray& eca)
	{
		for (auto entity : eca) {
			auto [system] = entity.Components.Tie<Components>();
			system->Update();
		}
	}
}
}
