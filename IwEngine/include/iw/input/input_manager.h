#pragma once

#include "iwinput.h"
#include "input_context.h"
#include "keyboard.h"
#include "mouse.h"

namespace iwinput {
	class IWINPUT_API input_manager {
	private:
		class impl_input_manager;
		impl_input_manager* m_impl;
	public:
		input_manager();
		~input_manager();

		/**
		* Sets the window handle. Needed for mouse.
		* @param window_handle A platform specific handle to the window.
		*/
		void set_window_info(IWI_HANDLE window_handle);

		/**
		* Sets if the cursor should reset for the active context every update.
		* recenter If the cursor should recenter.
		*/
		void recenter_cursor_on_update(bool recenter);

		/**
		* Updates every state with smoothing.
		*/
		void update(float delta_time);

		/**
		* Updates every state without smoothing.
		*/
		void update();

		/**
		* Creates a new device.
		* @return A reference to the device.
		*/
		template<typename DeviceType>
		DeviceType& create_device();

		/**
		* Creates a new keyboard.
		* @return A reference to the keyboard.
		*/
		template<>
		keyboard& create_device<keyboard>();

		/**
		* Creates a new mouse.
		* @return A reference of the new mouse.
		*/
		template<>
		mouse& create_device<mouse>();

		/**
		* Registers a new context and activates it.
		* @name The name of the context.
		* @return The newly created context.
		*/
		input_context* create_context(const char* name);

		/**
		* Returns the current active context.
		* @return The current active context or an expired pointer.
		*/
		input_context* active_context();

#ifdef IW_PLATFORM_WINDOWS
		/**
		* Handles message provided by the OS.
		* @param message The message to be handled.
		*/
		void handle_event(const MSG& event);
#endif /* _WIN32 */
	};
}
