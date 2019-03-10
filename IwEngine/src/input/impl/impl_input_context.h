#pragma once

#include "iw/input/iwinput.h"
#include "iw/input/input_state.h"
#include "iw/input/input_context.h"
#include <unordered_map>
#include <memory>

namespace iwinput {
	class impl_input_context : public input_context {
	private:
		struct input {
			uint key_id;
			uint device_id;
			//other information about input
		};

		std::unordered_map<uint, input> m_input;
		const std::shared_ptr<input_state> m_input_state;
	public:
		impl_input_context(const std::shared_ptr<input_state> input_state);

		/*
		* Maps a input to a custom id.
		* @param custom_id A custom id to refer to the input as.
		* @param key_id The id of the key.
		* @param device_id The id of the device to map this input to.
		* @return If the custom id got remapped.
		*/
		bool bind_input(uint custom_id, uint key_id, uint device_id);

		/*
		* Returns The current state of an input.
		* @param The inputs custom id.
		* @return The inputs current state.
		*/
		float get_state(uint custom_id) const;

		/*
		* Returns the raw input state.
		* @return The raw input state.
		*/
		inline const input_state& get_raw_states() const {
			return *m_input_state;
		}
	};
}
