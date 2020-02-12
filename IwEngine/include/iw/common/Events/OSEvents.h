#pragma once

#include "iw/common/iwcommon.h"
#include "EventGroups.h"
#include "iw/events/event.h"
#include "iw/util/enum/val.h"

#define IW_PLATFORM_MIN 1
#include "iw/common/Platform.h"

namespace iw {
namespace Engine {
	enum class OsEventType
		: short
	{
		GENERIC,
		INPUT
	};

	struct OsEvent
		: iw::event
	{
#ifdef IW_PLATFORM_WINDOWS
		unsigned int WindowId;
		unsigned int Message;
		WPARAM WParam;
		LPARAM LParam;

		OsEvent(
			unsigned int windowId,
			unsigned int message,
			WPARAM wparam,
			LPARAM lparam)
			: iw::event(iw::val(EventGroup::OS), iw::val(OsEventType::GENERIC))
			, WindowId(windowId)
			, Message(message)
			, WParam(wparam)
			, LParam(lparam)
		{}
#endif
	};
}

	using namespace Engine;
}
