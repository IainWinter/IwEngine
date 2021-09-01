#pragma once

#include "Device.h"

namespace iw {
namespace Input {
	struct KeyboardBase : Device
	{
		KeyboardBase(
			DeviceType type
		)
			: Device(type)
		{}

		IWINPUT_API static unsigned  Translate(InputName key);
		IWINPUT_API static InputName Translate(unsigned oskey);
	protected:
		static Translation translation; // Should just be in Device

		IWINPUT_API
		static Translation CreateTranslation();
	};

	struct Keyboard : KeyboardBase
	{
		Keyboard()
			: KeyboardBase(DeviceType::KEYBOARD)
		{}

		IWINPUT_API static bool KeyDown(InputName key);
		IWINPUT_API static bool KeyUp  (InputName key);

		IWINPUT_API
		static Keyboard* Create();
	};

	struct RawKeyboard : KeyboardBase
	{
		RawKeyboard()
			: KeyboardBase(DeviceType::RAW_KEYBOARD)
		{}

		IWINPUT_API
		static RawKeyboard* Create();
	};
}

	using namespace Input;
}
