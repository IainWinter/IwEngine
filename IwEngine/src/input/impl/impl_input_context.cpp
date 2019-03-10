#include "impl_input_context.h"

namespace iwinput {
	impl_input_context::impl_input_context(const std::shared_ptr<input_state> input_state)
		: m_input_state(input_state) {}

	bool impl_input_context::bind_input(uint custom_id, uint key_id, uint device_id) {
		bool overwritten = m_input.find(custom_id) != m_input.end();
		m_input[custom_id] = { key_id, device_id };

		return overwritten;
	}

	float impl_input_context::get_state(uint custom_id) const {
		float state = 0;
		if (m_input.find(custom_id) != m_input.end()) {
			const input& input = m_input.at(custom_id);
			state = m_input_state->get_state(input.key_id);
		}

		return state;
	}
}
