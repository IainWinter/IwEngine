#pragma once

#include "iw/engine/Events/Event.h"
#include "iw/log/logger.h"
#include "iw/engine/Platform.h"

namespace IwEngine {
	template<
		typename _event_t>
	_event_t Translate(
		int event,
		int wParam,
		long lParam)
	{
		LOW_WARNING << "No translation exists for event " << event
			<< " {wParam = " << wParam << ", lParam" << lParam << "}";

		return _event_t();
	}

	template<>
	WindowResizedEvent Translate(
		int event,
		int wParam,
		long lParam)
	{
		return WindowResizedEvent(LOWORD(lParam), HIWORD(lParam));
	}
}
