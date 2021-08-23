#include "iw/engine/System.h"
#include "iw/graphics/ParticleSystem.h"

#include "Assets.h"

struct BackgroundSystem : iw::SystemBase
{
	iw::ParticleSystem<iw::StaticParticle> m_stars;

	BackgroundSystem()
		: iw::SystemBase("BAckground")
	{}

	int Initialize() override;
	void Update() override;
};
