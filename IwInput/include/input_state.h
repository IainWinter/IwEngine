#pragma once

#include "iwinput.h"

namespace iwinput {
	class IWINPUT_API input_state {
	public:
		virtual ~input_state() {}

		/**
		* Returns the state of input.
		* @param state_id The id of the input.
		* @return The state of the input.
		*/
		virtual float get_state(const uint& state_id) const = 0;

		/**
		* Returns the smoothed state of input.
		* @param state_id The id of the input.
		* @return The smoothed state of the input.
		*/
		virtual float get_smoothed_state(const uint& state_id) const = 0;
	};
}