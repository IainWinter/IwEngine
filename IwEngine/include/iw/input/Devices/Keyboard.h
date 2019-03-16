#pragma once

#include "Device.h"
#include "iw/input/Events/InputEvent.h"

namespace IwInput {
	class KeyboardBase
		: public Device
	{
	public:
		KeyboardBase(
			InputCallback& callback)
			: Device(callback)
		{}

		virtual ~KeyboardBase() {}

		virtual void HandleEvent(
			OsEvent& event) = 0;
	};

	class Keyboard
		: public KeyboardBase
	{
	protected:
		static Translation CreateTranslation();

	public:
		Keyboard(
			InputCallback& callback)
			: KeyboardBase(callback)
		{}

		virtual ~Keyboard() {}

		virtual void HandleEvent(
			OsEvent& event) = 0;

		static InputName Translate(
			unsigned int oskey);

		static Keyboard* Create(
			InputCallback& callback);
	};
}
