#pragma once

#include "iw/engine/Events/Events.h"
#include "iw/log/logger.h"
#include "iw/common/Platform.h"

namespace iw {
namespace Engine {
	template<
		typename _event_t>
	_event_t Translate(
		unsigned windowId,
		void* handle,
		unsigned msg,
		size_t wParam,
		size_t lParam)
	{
		LOW_WARNING << "No translation exists for message "
			<< msg << " {wParam = " << wParam << ", lParam" << lParam << "}";

		return _event_t();
	}

	template<>
	WindowResizedEvent Translate(
		unsigned windowId,
		void* handle,
		unsigned msg,
		size_t wParam,
		size_t lParam)
	{
		return WindowResizedEvent(windowId, LOWORD(lParam), HIWORD(lParam));
	}
}
}
