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

#define APP_VARS iw::ref<iw::eventbus>       Bus;      \
			  iw::ref<iw::thread_pool>    Task;     \
			  iw::ref<iw::IAudioSpace>    Audio;    \
			  iw::ref<iw::AssetManager>   Asset;    \
			  iw::ref<iw::Console>        Console;  \
			  iw::ref<iw::InputManager>   Input;    \
			  iw::ref<iw::DynamicsSpace>  Physics;  \
			  iw::ref<iw::QueuedRenderer> Renderer; \
			  iw::ref<iw::Space>          Space;    \

#define APP_VARS_LIST iw::AppVars { Bus,      \
						  Task,     \
						  Audio,    \
						  Asset,    \
						  Console,  \
						  Input,    \
						  Physics,  \
						  Renderer, \
						  Space }   \

#define SET_APP_VARS void SetAppVars(         \
					iw::AppVars& vars)			  \
				{						      \
					Bus      = vars.Bus;	  \
					Task     = vars.Task;	  \
					Audio    = vars.Audio;	  \
					Asset    = vars.Asset;	  \
					Console  = vars.Console;  \
					Input    = vars.Input;	  \
					Physics  = vars.Physics;  \
					Renderer = vars.Renderer; \
					Space    = vars.Space;	  \
				}                             \

#define MAKE_APP_VARS iw::AppVars MakeAppVars() { \
					iw::AppVars vars;			  \
					vars.Bus      = Bus;	  \
					vars.Task     = Task;	  \
					vars.Audio    = Audio;	  \
					vars.Asset    = Asset;	  \
					vars.Console  = Console;  \
					vars.Input    = Input;	  \
					vars.Physics  = Physics;  \
					vars.Renderer = Renderer; \
					vars.Space    = Space;	  \
                           					  \
					return vars;			  \
				}						      \

namespace iw {
namespace Engine {
	struct AppVars {
		APP_VARS
	};
}

	using namespace Engine;
}
