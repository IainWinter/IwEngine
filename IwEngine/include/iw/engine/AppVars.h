#pragma once

#include "iw/events/eventbus.h"
#include "iw/util/thread/thread_pool.h"

#include "iw/audio/AudioSpace.h"
#include "iw/asset/AssetManager.h"
#include "iw/engine/Console.h"
#include "iw/input/InputManager.h"
#include "iw/physics/Dynamics/DynamicsSpace.h"
#include "iw/graphics/QueuedRenderer.h"
#include "iw/entity/Entity.h"

#define APP_VARS ref<eventbus>       Bus;      \
			  ref<thread_pool>    Task;     \
			  ref<AudioSpace>     Audio;    \
			  ref<AssetManager>   Asset;    \
			  ref<Console>        Console;  \
			  ref<InputManager>   Input;    \
			  ref<DynamicsSpace>  Physics;  \
			  ref<QueuedRenderer> Renderer; \
			  ref<Space>          Space;    \

#define SET_APP_VARS void SetAppVars(              \
					AppVars& vars)			 \
				{						 \
					Bus      = vars.Bus;	 \
					Task     = vars.Task;	 \
					Audio    = vars.Audio;	 \
					Asset    = vars.Asset;	 \
					Console  = vars.Console;	 \
					Input    = vars.Input;	 \
					Physics  = vars.Physics;	 \
					Renderer = vars.Renderer; \
					Space    = vars.Space;	 \
				}

#define MAKE_APP_VARS AppVars MakeAppVars() {      \
					AppVars vars;			 \
					vars.Bus      = Bus;	 \
					vars.Task     = Task;	 \
					vars.Audio    = Audio;	 \
					vars.Asset    = Asset;	 \
					vars.Console  = Console;	 \
					vars.Input    = Input;	 \
					vars.Physics  = Physics;	 \
					vars.Renderer = Renderer; \
					vars.Space    = Space;	 \
                           					 \
					return vars;			 \
				}						 \

namespace iw {
namespace Engine {
	struct AppVars {
		APP_VARS
	};
}

	using namespace Engine;
}
