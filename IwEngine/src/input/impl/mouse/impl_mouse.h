#pragma once

#include "iw/input/mouse.h"
#include "..\..\iwinput_events.h"
#include "..\impl_input_device.h"
#include <unordered_map>

namespace iwinput {
	class impl_mouse : public mouse, public impl_input_device {
	private:
		float m_width;
		float m_height;
		long m_pos_absolute_x;
		long m_pos_absolute_y;
		long m_pos_last_x;
		long m_pos_last_y;
		bool m_recenter;
		std::unordered_map<uint, mouse_input> m_translation;
	public:
		impl_mouse(iwinput_state_event_bus& event_bus, float width, float height);

		void process(const os_event& args);
		void process(const update_event& args);
		void process(const settings_change_event& args);

		float poll_state(uint state_id) { return 0; }

		inline void recenter_on_update(bool recenter) {
			m_recenter = recenter;
		}

		inline uint id() {
			return impl_input_device::id();
		}
	};
}
