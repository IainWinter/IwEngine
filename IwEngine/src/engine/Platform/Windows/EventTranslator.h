#pragma once

#include "iw/engine/Events/AllEvents.h"
#include "iw/log/logger.h"
#include "iw/engine/Platform.h"

namespace IwEngine {
	template<
		typename _event_t>
	_event_t* Translate(
		int event,
		size_t wParam,
		size_t lParam)
	{
		LOW_WARNING << "No translation exists for event " << event
			<< " {wParam = " << wParam << ", lParam" << lParam << "}";

		return nullptr;
	}

	template<>
	WindowResizedEvent* Translate(
		int event,
		size_t wParam,
		size_t lParam)
	{
		return new WindowResizedEvent(LOWORD(lParam), HIWORD(lParam));
	}
}
