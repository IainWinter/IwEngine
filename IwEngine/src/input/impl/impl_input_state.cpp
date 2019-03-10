#include "impl_input_state.h"
#include "../iwinput_events.h"

namespace iwinput {
	impl_input_state::impl_input_state() {
		m_state_count = KEYBOARD_INPUT_COUNT + MOUSE_INPUT_COUNT;
		m_states = new float[m_state_count];
		m_smoothed_states = new float[m_state_count];

		memset(m_states, 0, m_state_count * sizeof(float));
		memset(m_smoothed_states, 0, m_state_count * sizeof(float));
	}

	impl_input_state::~impl_input_state() {
		input_state::~input_state();
		delete m_states;
		delete m_smoothed_states;
	}

	void impl_input_state::process(const state_change_event& args) {
		m_states[args.key_id] = args.state;
	}

	void impl_input_state::process(const smoothed_state_change_event& args) {
		m_states[args.key_id] = args.state;
	}
}
