#pragma once

#include "iw/common/iwcommon.h"
#include "iw/events/event.h"
#include "EventGroups.h"

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
		: event
	{
#ifdef IW_PLATFORM_WINDOWS
		unsigned WindowId;
		HWND Handle;
		unsigned Message;
		WPARAM WParam;
		LPARAM LParam;

		OsEvent(
			unsigned windowId,
			HWND handle,
			unsigned message,
			WPARAM wparam,
			LPARAM lparam)
			: iw::event(iw::val(EventGroup::OS), iw::val(OsEventType::GENERIC))
			, WindowId(windowId)
			, Handle(handle)
			, Message(message)
			, WParam(wparam)
			, LParam(lparam)
		{}
#endif
	};
}

	using namespace Engine;
}
