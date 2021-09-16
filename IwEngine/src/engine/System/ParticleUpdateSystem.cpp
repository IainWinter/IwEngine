#include "iw/engine/Systems/ParticleUpdateSystem.h"

namespace iw {
namespace Engine {
	ParticleUpdateSystem::ParticleUpdateSystem()
		: SystemBase("Particle Update")
	{}

	int ParticleUpdateSystem::Initialize() {
		m_particleSystems = Space->MakeQuery<ParticleSystem<StaticParticle>>();
		//query->SetAny({ Space->GetComponent<Transform>() });

		return 0;
	}

	void ParticleUpdateSystem::Update() 
	{
		EntityComponentArray entities = Space->Query(m_particleSystems);

		for (EntityComponentData& entity : entities) 
		{
			auto [system] = entity.Components.Tie<Components>();
			system->Update();
		}
	}
}
}
