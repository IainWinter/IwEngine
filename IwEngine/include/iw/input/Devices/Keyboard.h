#pragma once

#include "Device.h"

namespace IW {
inline namespace Input {
	class IWINPUT_API KeyboardBase
		: public Device
	{
	protected:
		static Translation translation; // Should just be in Device

		static Translation CreateTranslation();

	public:
		KeyboardBase(
			DeviceType type)
			: Device(type)
		{}

		virtual ~KeyboardBase() = default;

		static unsigned int Translate(
			InputName key);

		static InputName Translate(
			unsigned int oskey);
	};

	class IWINPUT_API Keyboard
		: public KeyboardBase
	{
	public:
		Keyboard()
			: KeyboardBase(DeviceType::KEYBOARD)
		{}

		virtual ~Keyboard() = default;

		static bool KeyDown(
			InputName key);

		static bool KeyUp(
			InputName key);

		static Keyboard* Create();
	};

	class IWINPUT_API RawKeyboard
		: public KeyboardBase
	{
	public:
		RawKeyboard()
			: KeyboardBase(DeviceType::RAW_KEYBOARD)
		{}

		virtual ~RawKeyboard() = default;

		static RawKeyboard* Create();
	};
}
}
