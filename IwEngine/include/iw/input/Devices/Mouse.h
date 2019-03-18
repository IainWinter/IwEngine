#pragma once

#include "Device.h"
#include "iw/input/Events/InputEvent.h"

namespace IwInput {
	class MouseBase
		: public Device
	{
	public:
		MouseBase(
			InputCallback& callback)
			: Device(callback)
		{}

		virtual ~MouseBase() {}

		virtual void HandleEvent(
			OsEvent& event) = 0;
	};

	class Mouse
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
	};

#ifdef IW_PLATFORM_WINDOWS
	class RawMouse
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
