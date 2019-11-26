#include "iw/input/InputManager.h"
#include "iw/input/Devices/Mouse.h"
#include "iw/input/Events/InputEvents.h"
#include "iw/input/DeviceInput.h"
#include "iw/common/Events/OSEvents.h"

namespace IW {
	InputManager::InputManager(
		iw::ref<iw::eventbus>& bus)
		: m_bus(bus)
	{}

	void InputManager::HandleEvent(
		iw::event& e)
	{
		if (e.Type != OS) {
			return;
		}

		OsEvent& osevent = (OsEvent&)e;

		Context& context = *m_active.get();

		LOG_INFO << e.Category;

		for (iw::ref<Device>& device : context.Devices) {
			DeviceInput input = device->TranslateOsEvent(osevent);

			if (input.Name == INPUT_NONE) {
				continue;
			}

			bool active = !!input.State;

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

			context.State[input.Name] = input.State;

			// Dispatch the event as a Mouse or Keyboard event
			// Check actions table for a matching action event to also send out

			//ActionEvent action = context.Translator(input);

			if (input.Device == KEYBOARD) {
				if (active) {
					bool shifted = !!context.State[SHIFT];
					bool    caps = !!context.State.GetLock(CAPS_LOCK);

					char character = GetCharacter(input.Name, shifted, caps);
					if (character != '\0') {
						m_bus->push<KeyTypedEvent>(&context.State, input.Name, character);
					}
				}

				if (input.State != lastState) {
					m_bus->push<KeyEvent>(&context.State, input.Name, active);
				}
			}

			else if (input.Device == MOUSE) {
				if (input.Name == WHEEL) {
					m_bus->push<MouseWheelEvent>(&context.State, input.State);
				}

				else if (input.Name == MOUSEdX || input.Name == MOUSEdY
					  || input.Name == MOUSEX  || input.Name == MOUSEY)
				{
					m_bus->push<MouseMovedEvent>(&context.State,
						context.State[MOUSEX],  context.State[MOUSEY],
						context.State[MOUSEdX], context.State[MOUSEdY]);
				}

				else {
					m_bus->push<MouseButtonEvent>(&context.State, input.Name, active);
				}
			}
		}
	}

	void InputManager::CreateContext(
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
	}

	template<>
	iw::ref<Device>& InputManager::CreateDevice<Mouse>(
		std::string name)
	{
		iw::ref<Device> device(Mouse::Create(name));
		return TryAddDevice(name, device) ? device : nullptr;
	}

	template<>
	iw::ref<Device>& InputManager::CreateDevice<Keyboard>(
		std::string name)
	{
		iw::ref<Device> device(Keyboard::Create(name));
		return TryAddDevice(name, device) ? device : nullptr;
	}

#ifdef IW_PLATFORM_WINDOWS
	template<>
	iw::ref<Device>& InputManager::CreateDevice<RawMouse>(
		std::string name)
	{
		iw::ref<Device> device(RawMouse::Create(name));
		return TryAddDevice(name, device) ? device : nullptr;
	}

	template<>
	iw::ref<Device>& InputManager::CreateDevice<RawKeyboard>(
		std::string name)
	{
		iw::ref<Device> device(RawKeyboard::Create(name));
		return TryAddDevice(name, device) ? device : nullptr;
	}
#endif

	bool InputManager::TryAddDevice(
		std::string name,
		iw::ref<Device>& device)
	{
		iw::ref<Device> ret;
		for (iw::ref<Device>& d : m_devices) {
			if (d->Name == name) {
				ret = d;
				break;
			}
		}

		if (!ret) {
			m_devices.push_back(device);
			m_active->Devices.push_back(device);
		}

		return !!ret;
	}
}
