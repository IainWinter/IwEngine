#pragma once

#include "iw/common/Platform.h"

#include "iw/events/event.h"
#include "iw/common/iwcommon.h"
#include "iw/common/Events/EventGroups.h"

namespace iw {
namespace Engine {
	enum class OsEventType
		: short
	{
		GENERIC,
		INPUT
	};

	struct REFLECT OsEvent
		: event
	{
#ifdef IW_PLATFORM_WINDOWS

		NO_REFLECT HWND Handle;

		unsigned WindowId;
		unsigned Message;
		WPARAM WParam;
		LPARAM LParam;

		OsEvent(
			unsigned windowId,
			HWND handle,
			unsigned message,
			WPARAM wparam,
			LPARAM lparam
		)
			: event(val(EventGroup::OS), val(OsEventType::GENERIC))
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
