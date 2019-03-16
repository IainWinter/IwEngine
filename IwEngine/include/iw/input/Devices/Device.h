#pragma once

#include "iw/input/IwInput.h"
#include "iw/input/Events/InputEvent.h"
#include "iw/events/functional/callback.h"
#include "iw/util/set/sparse_set.h"

namespace IwInput {
	using Translation = iwutil::sparse_set<unsigned int, InputName>;

	class Device
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
			InputEvent event)
		{
			m_callback(event);
		}
	};
}
