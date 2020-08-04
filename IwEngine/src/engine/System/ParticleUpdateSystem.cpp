#include "iw/engine/Systems/ParticleUpdateSystem.h"

namespace iw {
namespace Engine {
	ParticleUpdateSystem::ParticleUpdateSystem()
		: SystemBase("Particle Update")
	{}

	int ParticleUpdateSystem::Initialize() {
		query = Space->MakeQuery<ParticleSystem<StaticParticle>>();
		//query->SetAny({ Space->GetComponent<Transform>() });

		return 0;
	}

	void ParticleUpdateSystem::Update() {
		auto entities = Space->Query(query);

		for (auto entity : entities) {
			auto [system] = entity.Components.Tie<Components>();

			system->Update();
		}
	}
}
}
