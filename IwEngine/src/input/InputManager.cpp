#include "iw/input/InputManager.h"
#include"iw/input/Devices/Mouse.h"

namespace IW {
#ifdef IW_PLATFORM_WINDOWS
	void InputManager::HandleEvent(
		int wid,
		int msg,
		size_t wParam,
		size_t lParam)
	{
		m_active->HandleOsEvent({ wid, msg, wParam, lParam });
	}
#endif

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
			m_contexts.emplace_back(name, width, height);
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
		iw::ref<Device> device;
		for (iw::ref<Device>& d : m_devices) {
			if (d->Name == name) {
				device = d;
				break;
			}
		}

		if (!device) {
			m_devices.push_back(device);
		}

		return device.use_count() == 0;
	}
}
