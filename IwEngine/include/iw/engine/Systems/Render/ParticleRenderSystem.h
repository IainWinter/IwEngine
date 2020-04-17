#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/graphics/ParticleSystem.h"
#include "iw/graphics/Scene.h"

namespace iw {
namespace Engine {
	class ParticleRenderSystem
		: public System<Transform, ParticleSystem<StaticParticle>> // need to figure out how to do the components for graphics stuff, they should all have some sort of wrapper
	{
	public:
		struct Components {
			Transform* Transform;
			ParticleSystem<StaticParticle>* ParticleSystem;
		};

		Scene* m_scene;

	public:
		IWENGINE_API
		ParticleRenderSystem(
			Scene* m_scene);

		IWENGINE_API
		void Update(
			EntityComponentArray& view);
	};
}

	using namespace Engine;
}
