#include "impl_input_manager.h"
#include "input_device.h"
#include "keyboard\impl_keyboard.h"
#include "mouse\impl_mouse.h"
#include <exception>

namespace iwinput {
	input_manager::impl_input_manager::impl_input_manager()
		: m_device_event_bus(), m_state_change_event_bus()
	{
		m_input_state = std::make_shared<impl_input_state>();
		m_state_change_event_bus.subscribe<impl_input_state>(m_input_state);
	}

	input_manager::impl_input_manager::~impl_input_manager() {
		for (auto itr : m_contexts) {
			delete itr.second;
		}
	}

	void input_manager::impl_input_manager::set_window_info(IWI_HANDLE window_handle) {
		m_window_handle = window_handle;
#ifdef IWI_WINDOWS
		RECT r;
		GetWindowRect(m_window_handle, &r);
		m_width =  r.right - r.left;
		m_height = r.bottom - r.top;
#endif
	}

	void input_manager::impl_input_manager::recenter_cursor_on_update(bool recenter) {
		settings_change_event args{
			recenter
		};

		m_device_event_bus.publish<settings_change_event>(args);
	}

	void input_manager::impl_input_manager::update() {
		update(0);
	}

	void input_manager::impl_input_manager::update(float delta_time) {
		update_event args{ delta_time, m_window_handle };
		m_device_event_bus.publish<update_event>(args);
	}

	void input_manager::impl_input_manager::handle_event(const os_event& args) {
		m_device_event_bus.publish<os_event>(args);
	}

	input_device& input_manager::impl_input_manager::create_device(const device_type& device_type) {
		std::shared_ptr<impl_input_device> device;
		switch (device_type) {
		case KEYBOARD:
			device = std::make_shared<impl_keyboard>(m_state_change_event_bus);
			break;
		case MOUSE:
			device = std::make_shared<impl_mouse>(m_state_change_event_bus, m_width, m_height);
			break;
		default:
			throw std::exception("Failed to create unknow device. No memory has been leaked.");
			break;
		}

		m_device_event_bus.subscribe<impl_input_device>(device);
		m_devices.emplace(device->id(), device);

		return *device;
	}

	input_context* input_manager::impl_input_manager::create_context(const char* name) {
		impl_input_context* context = new impl_input_context(m_input_state);
		m_active_context = context;
		m_contexts.emplace(name, context);

		return m_active_context;
	}
}