#pragma once

#include "iw/input/IwInput.h"
#include "iw/input/Events/InputEvent.h"
#include "iw/events/callback.h"
#include "iw/util/set/tofrom_set.h"

namespace IW {
inline namespace Input {
	using Translation = iw::tofrom_set<unsigned int, InputName>;

	class IWINPUT_API Device
	{
	private:
		InputCallback m_callback;

	public:
		Device(
			InputCallback& callback)
			: m_callback(callback)
		{}

		virtual ~Device() {}

		virtual void HandleEvent(
			OsEvent& event) = 0;

		inline void SetCallback(
			InputCallback callback)
		{
			m_callback = callback;
		}
	protected:
		void Callback(
			InputEvent& event)
		{
			m_callback(event);
		}
	};
}
}
