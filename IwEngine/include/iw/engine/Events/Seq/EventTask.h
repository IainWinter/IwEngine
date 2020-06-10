#pragma once

#include "iw/engine/Core.h"

#include "iw/events/seq/event_task.h"
#include "iw/entity/Space.h"
#include "iw/graphics/QueuedRenderer.h"
#include "iw/asset/AssetManager.h"
#include "iw/physics/Dynamics/DynamicsSpace.h"
#include "iw/audio/AudioSpace.h"
#include "iw/events/eventbus.h"
#include "iw/util/memory/ref.h"

namespace iw {
namespace Engine {
	struct EventTask
		: event_task
	{
	protected:
		ref<Space>          Space;
		ref<QueuedRenderer> Renderer;
		ref<AssetManager>   Asset;
		ref<DynamicsSpace>  Physics;
		ref<AudioSpace>     Audio;
		ref<eventbus>       Bus;

	private:
		friend class EventSequence;

		void SetVars(
			ref<iw::Space>      space,
			ref<QueuedRenderer> renderer,
			ref<AssetManager>   asset,
			ref<DynamicsSpace>  physics,
			ref<AudioSpace>     audio,
			ref<eventbus>       bus)
		{
			Space    = space;
			Renderer = renderer;
			Asset    = asset;
			Physics  = physics;
			Audio    = audio;
			Bus      = bus;
		}
	};
}

using namespace Engine;
}
