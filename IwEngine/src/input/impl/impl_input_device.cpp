#include "impl_input_device.h"

namespace iwinput {
	uint impl_input_device::next_device_id = 0;

	impl_input_device::impl_input_device(iwinput_state_event_bus& event_bus)
		: m_state_event_bus(event_bus), m_device_id(next_device_id++) {}

	void impl_input_device::handle_state_change(uint device_id, uint key_id, float state) {
		state_change_event args {
			device_id, 
			key_id,
			state 
		};

		m_state_event_bus.publish<state_change_event>(args);
	}
}
