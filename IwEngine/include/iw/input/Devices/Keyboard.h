#pragma once

#include "Device.h"
#include "iw/input/Events/InputEvent.h"

namespace IwInput {
	class IWINPUT_API KeyboardBase
		: public Device
	{
	protected:
		// Should just be in Device
		static Translation translation;

		static Translation CreateTranslation();

	public:
		KeyboardBase(
			InputCallback& callback)
			: Device(callback)
		{}

		virtual ~KeyboardBase() {}

		virtual void HandleEvent(
			OsEvent& event) = 0;

		static unsigned int Translate(
			InputName key);

		static InputName Translate(
			unsigned int oskey);
	};

	class IWINPUT_API Keyboard
		: public KeyboardBase
	{
	public:
		Keyboard(
			InputCallback& callback)
			: KeyboardBase(callback)
		{}

		virtual ~Keyboard() {}

		virtual void HandleEvent(
			OsEvent& event) = 0;

		static Keyboard* Create(
			InputCallback& callback);

		static bool KeyDown(
			InputName key);

		static bool KeyUp(
			InputName key);
	};

#ifdef IW_PLATFORM_WINDOWS
	class IWINPUT_API RawKeyboard
		: public KeyboardBase
	{
	public:
		RawKeyboard(
			InputCallback& callback)
			: KeyboardBase(callback)
		{}

		virtual ~RawKeyboard() {}

		virtual void HandleEvent(
			OsEvent& event) = 0;

		static RawKeyboard* Create(
			InputCallback& callback);
	};
#endif
}
