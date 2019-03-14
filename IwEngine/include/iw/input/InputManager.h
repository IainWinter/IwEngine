#pragma once

#include "DeviceManager.h"
#include "Events/InputEvent.h"
#include "iw/events/functional/callback.h"

namespace IwInput {
	class IWINPUT_API InputManager {
	public:
		using InputCallback = iwevents::callback<InputEvent&>; //temp
	private:
		DeviceManager m_deviceManager;
		struct {
			InputCallback m_callback;
		};

	public:
		InputManager() = default;

#ifdef IW_PLATFORM_WINDOWS
		void HandleEvent(
			int wid,
			int msg,
			short wParam,
			int lParam);
#endif

		void HandleInput(
			InputEvent input);

		inline void SetCallback(
			InputCallback callback)
		{
			m_callback = callback;
		}

	private:
	};
}
