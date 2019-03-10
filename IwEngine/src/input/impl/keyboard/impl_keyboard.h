#pragma once

#include "iw/input/keyboard.h"
#include "..\..\iwinput_events.h"
#include "..\impl_input_device.h"
#include <unordered_map>

namespace iwinput {
	class impl_keyboard : public keyboard, public impl_input_device {
	private:
		std::unordered_map<uint, keyboard_input> m_translation;
	public:
		impl_keyboard(iwinput_state_event_bus& event_bus);

		void process(const os_event& args);
		void process(const update_event& delta_time) {}
		void process(const settings_change_event& args) {}

		float poll_state(uint state_id) { return 0; }

		inline uint id() {
			return impl_input_device::id();
		}
	};
}
