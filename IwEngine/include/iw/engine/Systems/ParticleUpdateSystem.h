#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/graphics/ParticleSystem.h"

namespace iw {
namespace Engine {
	class ParticleUpdateSystem
		: public System<ParticleSystem<StaticParticle>> // need to figure out how to do the components for graphics stuff, they should all have some sort of wrapper
	{
	public:
		struct Components {
			ParticleSystem<StaticParticle>* ParticleSystem;
		};

	public:
		IWENGINE_API
		ParticleUpdateSystem();

		IWENGINE_API
		void Update(
			EntityComponentArray& view);
	};
}

	using namespace Engine;
}
