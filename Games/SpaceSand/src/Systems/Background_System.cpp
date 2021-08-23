#include "Systems/Background_System.h"

int BackgroundSystem::Initialize()
{
	// load star mesh

	m_stars.SetParticleMesh(A_mesh_star);

	m_stars.SetUpdate([&](
		iw::ParticleSystem<iw::StaticParticle>* system, 
		iw::Particle      <iw::StaticParticle>* particles, 
		unsigned count) 
	{
		system->SpawnParticle(iw::Transform(glm::vec3(iw::randfs() * 1, 1, 0), glm::vec3(.01)));

		for (int i = 0; i < count; i++)
		{
			particles[i].Transform.Position.y -= .1f * iw::DeltaTime();
		}

		return true;
	});

	return 0;
}

void BackgroundSystem::Update()
{
	m_stars.Update();
	m_stars.UpdateParticleMesh();
}
