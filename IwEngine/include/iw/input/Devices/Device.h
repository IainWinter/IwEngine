#pragma once

#include "iw/input/IwInput.h"
#include "iw/input/Events/InputEvent.h"
#include "iw/events/functional/callback.h"

namespace IwInput {
	class IDevice {
	public:
		using InputCallback = iwevents::callback<InputEvent&>;

		virtual ~IDevice() {}

		virtual void HandleEvent(
			OsEvent event) = 0;

		virtual void SetCallback(
			InputCallback callback) = 0;
	};

	class Device
		: public IDevice
	{
	private:
		InputCallback m_callback;

	public:
		virtual ~Device() {}

		virtual void HandleEvent(
			OsEvent event) = 0;

		inline void SetCallback(
			InputCallback callback)
		{
			m_callback = callback;
		}
	protected:
		void Callback(
			InputEvent event)
		{
			m_callback(event);
		}
	};
}
