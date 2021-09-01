#pragma once

#include "Device.h"
#include "iw/math/matrix.h" // maybe just use the forward declared version

namespace iw {
namespace Input {
	struct MouseBase : Device
	{
		MouseBase(DeviceType type)
			: Device(type)
		{}

		IWINPUT_API static unsigned  Translate(InputName key);
		IWINPUT_API static InputName Translate(unsigned oskey);
	protected:
		static Translation translation;

		IWINPUT_API
		static Translation CreateTranslation();
	};

	struct Mouse : MouseBase
	{
		Mouse()
			: MouseBase(DeviceType::MOUSE)
		{}

		IWINPUT_API static bool ButtonDown(InputName button);
		IWINPUT_API static bool ButtonUp  (InputName button);

		IWINPUT_API static vec2 ScreenPos();
		IWINPUT_API static vec2 ClientPos();

		IWINPUT_API
		static Mouse* Create();
	};

	struct RawMouse : MouseBase
	{
		RawMouse()
			: MouseBase(DeviceType::RAW_MOUSE)
		{}

		IWINPUT_API
		static RawMouse* Create();
	};
}

	using namespace Input;
}
