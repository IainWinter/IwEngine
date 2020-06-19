#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/AppVars.h"
#include "iw/events/seq/event_task.h"

namespace iw {
namespace Engine {
	struct EventTask
		: event_task
	{
	protected:
		APP_VARS

	private:
		friend class EventSequence;

		SET_APP_VARS
	};
}

using namespace Engine;
}
