#pragma once

#include "iw/input/input_state.h"

namespace iwinput {
	class IWINPUT_API impl_input_state : public input_state {
	private:
		uint m_state_count;
		float* m_states;
		float* m_smoothed_states;
	public:
		impl_input_state();
		~impl_input_state();

		void process(const state_change_event& args);
		void process(const smoothed_state_change_event& args);

		/**
		* Returns the state of input.
		* @param state_id The id of the input.
		* @return The state of the input.
		*/
		inline float get_state(const uint& state_id) const {
			if (m_state_count > state_id && state_id >= 0) {
				return m_states[state_id];
			}

			return 0;
		}

		/**
		* Returns the smoothed state of input.
		* @param state_id The id of the input.
		* @return The smoothed state of the input.
		*/
		inline float get_smoothed_state(const uint& state_id) const {
			if (m_state_count > state_id && state_id >= 0) {
				return m_smoothed_states[state_id];
			}

			return 0;
		}
	};
}
