#pragma once

#include "iw/input/input_device.h"
#include "../iwinput_events.h"

namespace iwinput {
	class impl_input_device : public input_device {
	private:
		iwinput_state_event_bus& m_state_event_bus;

		const uint m_device_id;
		static uint next_device_id;
	protected:
		void handle_state_change(uint device_id, uint key_id, float state);
	public:
		impl_input_device(iwinput_state_event_bus& event_bus);

		virtual void process(const os_event& args) = 0;
		virtual void process(const update_event& args) = 0;
		virtual void process(const settings_change_event& args) = 0;
		virtual float poll_state(uint state_id) = 0;

		inline uint id() {
			return m_device_id;
		}
	};
}
