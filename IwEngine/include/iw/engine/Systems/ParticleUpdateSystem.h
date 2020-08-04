#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/graphics/ParticleSystem.h"

namespace iw {
namespace Engine {
	class ParticleUpdateSystem
		: public SystemBase // need to figure out how to do the components for graphics stuff, they should all have some sort of wrapper
	{
	public:
		struct Components {
			ParticleSystem<StaticParticle>* ParticleSystem;
		};

		iw::ref<ComponentQuery> query;

	public:
		IWENGINE_API
		ParticleUpdateSystem();

		IWENGINE_API
		int Initialize() override;

		IWENGINE_API
		void Update() override;
	};
}

	using namespace Engine;
}
