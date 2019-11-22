#pragma once

#include "Device.h"
#include "iw/input/Events/InputEvent.h"

namespace IW {
inline namespace Input {
	class IWINPUT_API KeyboardBase
		: public Device
	{
	protected:
		// Should just be in Device
		static Translation translation;
		static Translation CreateTranslation();

	public:
		KeyboardBase(
			std::string name);

		virtual ~KeyboardBase() {}

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
			std::string name);

		virtual ~Keyboard() {}

		static bool KeyDown(
			InputName key);

		static bool KeyUp(
			InputName key);

		static Keyboard* Create(
			std::string name);
	};

#ifdef IW_PLATFORM_WINDOWS
	class IWINPUT_API RawKeyboard
		: public KeyboardBase
	{
	public:
		RawKeyboard(
			std::string name);

		virtual ~RawKeyboard() {}

		static RawKeyboard* Create(
			std::string name);
	};
#endif
}
}
