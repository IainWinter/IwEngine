#pragma once

#include "input_state.h"

namespace iwinput {
	class IWINPUT_API input_context {
	public:
		virtual ~input_context() {}

		/*
		* Maps a input to a custom id.
		* @param custom_id A custom id to refer to the input as.
		* @param key_id The id of the key.
		* @param device_id The id of the device to map this input to.
		* @return If the custom id got remapped.
		*/
		virtual bool bind_input(uint custom_id, uint key_id, uint device_id) = 0;

		/*
		* Returns The current state of an input.
		* @param The inputs custom id.
		* @return The inputs current state.
		*/
		virtual float get_state(uint custom_id) const = 0;

		/*
		* Returns the raw input state.
		* @return The raw input state.
		*/
		virtual const input_state& get_raw_states() const = 0;

		//TODO: Make settings more based of off which context is active rather than the overall program. Need to have eventbus for setting settings and
		// knowledge of if itself is active. Could be givin the device event bus and storing is active bus. Also could send an event to a context bus telling all
		// other ones to deactivate which could fully separate the manager from the context. 
		///**
		//* Sets if the cursor should be set to the center of the window when input_manager::update is called.
		//* @param recenter If the cursor should be recentered.
		//*/
		//virtual void recenter_curcor_on_update(bool recenter) = 0;
	};
}