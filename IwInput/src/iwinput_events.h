#pragma once

#include "iwinput.h"
#include "event_bus.h"

namespace iwinput {
	struct update_event;
	struct os_event;
	struct state_change_event;
	struct smoothed_state_change_event;
	struct settings_change_event;

	typedef iwevents::event_bus<update_event, os_event, settings_change_event> iwinput_device_event_bus;
	typedef iwevents::event_bus<state_change_event, smoothed_state_change_event> iwinput_state_event_bus;

	struct update_event {
		const float delta_time;
		const IWI_HANDLE hwnd;
	};

	struct os_event {
		const IWI_HANDLE hwmd;
		const uint code;
		const long lparam;
		const uint wparam;
	};

	struct state_change_event {
		uint device_id;
		uint key_id;
		float state;
	};

	struct smoothed_state_change_event {
		uint device_id;
		uint key_id;
		float state;
	};

	struct settings_change_event {
		bool mouse_locked;
	};
}
