#pragma once

#include "Device.h"
#include "iw/input/Events/InputEvent.h"

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
			InputCallback& callback)
			: Device(callback)
		{}

		virtual ~MouseBase() {}

		virtual void HandleEvent(
			OsEvent& event) = 0;

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
			InputCallback& callback)
			: MouseBase(callback)
		{}

		virtual ~Mouse() {}

		virtual void HandleEvent(
			OsEvent& event) = 0;

		static Mouse* Create(
			InputCallback& callback);

		static bool ButtonDown(
			InputName button);

		static bool ButtonUp(
			InputName button);
	};

#ifdef IW_PLATFORM_WINDOWS
	class IWINPUT_API RawMouse
		: public MouseBase
	{
	public:
		RawMouse(
			InputCallback& callback)
			: MouseBase(callback)
		{}

		virtual ~RawMouse() {}

		virtual void HandleEvent(
			OsEvent& event) = 0;

		static RawMouse* Create(
			InputCallback& callback);
	};
#endif
}
}
