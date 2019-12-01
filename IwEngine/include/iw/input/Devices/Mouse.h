#pragma once

#include "Device.h"

namespace IW {
inline namespace Input {
	class IWINPUT_API MouseBase
		: public Device
	{
	protected:
		static Translation translation;

		static Translation CreateTranslation();

	public:
		MouseBase(
			std::string name);

		virtual ~MouseBase() {}

		static unsigned int Translate(
			InputName key);

		static InputName Translate(
			unsigned int oskey);
	};

	class IWINPUT_API Mouse
		: public MouseBase
	{
	public:
		Mouse(
			std::string name);

		virtual ~Mouse() {}

		static bool ButtonDown(
			InputName button);

		static bool ButtonUp(
			InputName button);

		static Mouse* Create(
			std::string name);
	};

#ifdef IW_PLATFORM_WINDOWS
	class IWINPUT_API RawMouse
		: public MouseBase
	{
	public:
		RawMouse(
			std::string name);

		virtual ~RawMouse() {}

		static RawMouse* Create(
			std::string name);
	};
#endif
}
}
