#pragma once

#include "EventGroups.h"
#include "iw/common/iwcommon.h"
#include "iw/events/event.h"
#include <vcruntime.h>

namespace IW {
inline namespace Engine {
	enum OsEventType {
		GENERIC
	};

	struct OsEvent
		: iw::event
	{
#ifdef IW_PLATFORM_WINDOWS
		unsigned int WindowId;
		unsigned int Message;
		size_t   WParam;
		intptr_t LParam;

		OsEvent(
			unsigned int windowId,
			unsigned int message,
			size_t wparam,
			intptr_t lparam)
			: iw::event(OS, GENERIC)
			, WindowId(windowId)
			, Message(message)
			, WParam(wparam)
			, LParam(lparam)
		{}
#endif
	};
}
}
