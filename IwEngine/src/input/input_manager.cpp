#include "iw/input/input_manager.h"
#include "impl/impl_input_manager.h"

namespace iwinput {
	input_manager::input_manager()
		: m_impl(new impl_input_manager()) {}

	input_manager::~input_manager() {
		delete m_impl;
	}

	void input_manager::set_window_info(IWI_HANDLE window_handle) {
		m_impl->set_window_info(window_handle);
	}

	void input_manager::recenter_cursor_on_update(bool recenter) {
		m_impl->recenter_cursor_on_update(recenter);
	}

	void input_manager::update(float delta_time) {
		m_impl->update(delta_time);
	}

	void input_manager::update() {
		m_impl->update();
	}

	template<>
	keyboard& input_manager::create_device<keyboard>() {
		return (keyboard&)m_impl->create_device(KEYBOARD);
	}

	template<>
	mouse& input_manager::create_device<mouse>() {
		return (mouse&)m_impl->create_device(MOUSE);
	}

	input_context* input_manager::create_context(const char* name) {
		return m_impl->create_context(name);
	}

	input_context* input_manager::active_context() {
		return m_impl->active_context();
	}

#ifdef IW_PLATFORM_WINDOWS
	void input_manager::handle_event(const MSG& event) {
		os_event args {
			event.hwnd,
			event.message,
			event.lParam,
			event.wParam
		};

		m_impl->handle_event(args);
	}
#endif /* _WIN32 */
}
