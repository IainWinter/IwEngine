#pragma once

#include "iw/engine/Events/Events.h"
#include "iw/log/logger.h"
#include "iw/common/Platform.h"

namespace IW {
	template<
		typename _event_t>
	_event_t Translate(
		int event,
		int windowId,
		size_t wParam,
		size_t lParam)
	{
		LOW_WARNING << "No translation exists for event " << event
			<< " {wParam = " << wParam << ", lParam" << lParam << "}";

		return _event_t();
	}

	template<>
	IW::WindowResizedEvent Translate(
		int event,
		int windowId,
		size_t wParam,
		size_t lParam)
	{
		return IW::WindowResizedEvent(windowId, LOWORD(lParam), HIWORD(lParam));
	}
}
