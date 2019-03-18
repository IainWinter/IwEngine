#pragma once

#include "DeviceManager.h"
#include "ContextManager.h"
#include "Events/InputEvent.h"
#include "iw/events/functional/callback.h"

namespace IwInput {
	class IWINPUT_API InputManager {
	private:
		DeviceManager  m_deviceManager;
		ContextManager m_contextManager;

	public:
		InputManager() = default;

#ifdef IW_PLATFORM_WINDOWS
		void HandleEvent(
			int wid,
			int msg,
			short wParam,
			int lParam);
#endif

		void CreateContext(
			unsigned int windowId);

		void CreateContext(
			unsigned int windowId,
			float width,
			float height);

		template<
			typename _device_T>
		void CreateDevice()
		{
			LOG_WARNING << "Attempted to create invalid device!";
		}

		template<>
		void CreateDevice<Mouse>();

		template<>
		void CreateDevice<Keyboard>();

#ifdef IW_PLATFORM_WINDOWS
		template<>
		void CreateDevice<RawMouse>();
#endif

		void SetMouseWheelCallback(
			unsigned int windowId,
			MouseWheelCallback callback);

		void SetMouseMovedCallback(
			unsigned int windowId,
			MouseMovedCallback callback);

		void SetMouseButtonCallback(
			unsigned int windowId,
			MouseButtonCallback callback);

		void SetKeyCallback(
			unsigned int windowId,
			KeyCallback callback);
	};
}
