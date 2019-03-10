#pragma once

#include "impl_input_device.h"
#include "impl_input_context.h"
#include "impl_input_state.h"
#include "../iwinput_events.h"
#include "iw/input/input_manager.h"
#include <unordered_map>

namespace iwinput {
	class input_manager::impl_input_manager {
	private:
		std::unordered_map<uint, std::shared_ptr<impl_input_device>> m_devices;

		std::unordered_map<const char*, impl_input_context*> m_contexts;
		impl_input_context* m_active_context;

		std::shared_ptr<impl_input_state> m_input_state;

		iwinput_device_event_bus m_device_event_bus;
		iwinput_state_event_bus m_state_change_event_bus;

		IWI_HANDLE m_window_handle;
		uint m_width;
		uint m_height;
	public:
		impl_input_manager();
		~impl_input_manager();

		void set_window_info(IWI_HANDLE window_handle);
		void recenter_cursor_on_update(bool recenter);
		void update();
		void update(float delta_time);
		void handle_event(const os_event& args);
		input_device& create_device(const device_type& device);
		input_context* create_context(const char* name);

		inline input_context* active_context() {
			return m_active_context;
		}
	};
}
