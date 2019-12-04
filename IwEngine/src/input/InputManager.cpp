#include "iw/input/InputManager.h"
#include "iw/input/Devices/Mouse.h"
#include "iw/input/Events/InputEvents.h"
#include "iw/input/DeviceInput.h"
#include "iw/common/Events/OSEvents.h"
#include "iw/common/Platform.h"

namespace IW {
#ifdef IW_PLATFORM_WINDOWS
	void SetRawInputEventLParam(
		OsEvent& e)
	{
		const size_t rihSize = sizeof(RAWINPUTHEADER);
		const HRAWINPUT rptr = (HRAWINPUT)e.LParam;

		UINT dwSize = 0;
		GetRawInputData(rptr, RID_INPUT, NULL, &dwSize, rihSize);

		if (dwSize == 0) {
			LOG_WARNING << "Invalid raw input device! HRAWINPUT " << e.LParam;
			e.LParam = 0;
			return;
		}

		LPBYTE lpb = new BYTE[dwSize];

		if (GetRawInputData(rptr, RID_INPUT, lpb, &dwSize, rihSize) != dwSize) {
			LOG_WARNING << "GetRawInputData does not return correct size!";
			e.LParam = 0;
			return;
		}

		e.LParam = (LPARAM)lpb;
	}
#endif

	InputManager::InputManager(
		iw::ref<iw::eventbus>& bus)
		: m_bus(bus)
	{}

	void InputManager::HandleEvent(
		iw::event& e)
	{
		if (e.Group != iw::val(EventGroup::OS) || !m_active) {
			return;
		}

		OsEvent& osevent = (OsEvent&)e;
		Context& context = *m_active.get();

		if (e.Type == iw::val(OsEventType::INPUT)) {
			SetRawInputEventLParam(osevent);
		}
		
		if (osevent.LParam == 0) {
			return;
		}

		for (iw::ref<Device>& device : context.Devices) {
			DeviceInput input = device->TranslateOsEvent(osevent);

			if (input.Name == INPUT_NONE) {
				continue;
			}

			bool active  = !!input.State;
			bool active2 = !!input.State2;

			if (active) {
				switch (input.Name) {
					case CAPS_LOCK:   context.State.ToggleLock(CAPS_LOCK);   break;
					case NUM_LOCK:    context.State.ToggleLock(NUM_LOCK);    break;
					case SCROLL_LOCK: context.State.ToggleLock(SCROLL_LOCK); break;
				}
			}

			// States

			float lastState = context.State[input.Name];

			switch (input.Name) {
				case MOUSEdX: context.State[MOUSEX] += input.State; break;
				case MOUSEdY: context.State[MOUSEY] += input.State; break;
				case MOUSEX:  context.State[MOUSEdX] = input.State - lastState; break;
				case MOUSEY:  context.State[MOUSEdY] = input.State - lastState; break;
			}

			context.State[input.Name]  = input.State;

			if (active2) {
				float lastState2 = context.State[input.Name2];

				switch (input.Name2) {
					case MOUSEdX: context.State[MOUSEX] += input.State2; break;
					case MOUSEdY: context.State[MOUSEY] += input.State2; break;
					case MOUSEX:  context.State[MOUSEdX] = input.State2 - lastState2; break;
					case MOUSEY:  context.State[MOUSEdY] = input.State2 - lastState2; break;
				}

				context.State[input.Name2] = input.State2;
			}

			// Dispatch the event as a Mouse or Keyboard event
			// Check actions table for a matching action event to also send out

			//ActionEvent action = context.Translator(input);

			if (input.Device == KEYBOARD) {
				if (active) {
					bool shifted = !!context.State[SHIFT];
					bool    caps = !!context.State.GetLock(CAPS_LOCK);

					char character = GetCharacter(input.Name, shifted, caps);
					if (character != '\0') {
						m_bus->push<KeyTypedEvent>(input.Device, &context.State, input.Name, character);
					}
				}

				if (input.State != lastState) {
					m_bus->push<KeyEvent>(input.Device, &context.State, input.Name, active);
				}
			}

			else if (input.Device == MOUSE) {
				if (input.Name == WHEEL) {
					m_bus->push<MouseWheelEvent>(input.Device, &context.State, input.State);
				}

				else if (input.Name == MOUSEdX || input.Name == MOUSEdY
					  || input.Name == MOUSEX  || input.Name == MOUSEY)
				{
					m_bus->push<MouseMovedEvent>(
						input.Device, &context.State,
						context.State[MOUSEX],  context.State[MOUSEY],
						context.State[MOUSEdX], context.State[MOUSEdY]);
				}

				else {
					m_bus->push<MouseButtonEvent>(input.Device, &context.State, input.Name, active);
				}
			}
		}
	}

	iw::ref<Context>& InputManager::CreateContext(
		std::string name,
		float width,
		float height)
	{
		iw::ref<Context> context;
		for (iw::ref<Context>& c : m_contexts) {
			if (c->Name == name) {
				context = c;
				break;
			}
		}

		if (!context) {
			context = std::make_shared<Context>(name, width, height);
			m_active = m_contexts.emplace_back(context);
		}

		return context;
	}

	template<>
	iw::ref<Device>& InputManager::CreateDevice<Mouse>() {
		iw::ref<Device> device(Mouse::Create());
		return TryAddDevice(device) ? device : nullptr;
	}

	template<>
	iw::ref<Device>& InputManager::CreateDevice<Keyboard>() {
		iw::ref<Device> device(Keyboard::Create());
		return TryAddDevice(device) ? device : nullptr;
	}

	template<>
	iw::ref<Device>& InputManager::CreateDevice<RawMouse>() {
		iw::ref<Device> device(RawMouse::Create());
		return TryAddDevice(device) ? device : nullptr;
	}

	template<>
	iw::ref<Device>& InputManager::CreateDevice<RawKeyboard>() {
		iw::ref<Device> device(RawKeyboard::Create());
		return TryAddDevice(device) ? device : nullptr;
	}

	bool InputManager::TryAddDevice(
		iw::ref<Device>& device)
	{
		iw::ref<Device> ret;
		for (iw::ref<Device>& d : m_devices) {
			if (d->Type == device->Type) {
				ret = d;
				break;
			}
		}

		if (!ret) {
			m_devices.push_back(device);
		}

		return !!ret;
	}
}
