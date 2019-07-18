#include "iw/input/DeviceManager.h"

namespace IwInput {
	void SpecialProcess(OsEvent& event);

	void DeviceManager::HandleEvent(
		OsEvent event)
	{
		if (this) {
			SpecialProcess(event);
			for (Device* device : m_devices) {
				device->HandleEvent(event);
			}
		}
	}

	template<>
	void DeviceManager::CreateDevice<Mouse>(
		InputCallback callback)
	{
		m_devices.emplace_back(Mouse::Create(callback));
	}

	template<>
	void DeviceManager::CreateDevice<Keyboard>(
		InputCallback callback)
	{
		m_devices.emplace_back(Keyboard::Create(callback));
	}

#ifdef IW_PLATFORM_WINDOWS
	template<>
	void DeviceManager::CreateDevice<RawMouse>(
		InputCallback callback)
	{
		m_devices.emplace_back(RawMouse::Create(callback));
	}

	template<>
	void DeviceManager::CreateDevice<RawKeyboard>(
		InputCallback callback)
	{
		m_devices.emplace_back(RawKeyboard::Create(callback));
	}
#endif

#ifdef IW_PLATFORM_WINDOWS
#include <Windows.h>
	void SpecialProcess(OsEvent& event) {
		if (event.Message != WM_INPUT) {
			return;
		}

		UINT dwSize = 0;
		GetRawInputData((HRAWINPUT)event.LParam, RID_INPUT, NULL, &dwSize,
			sizeof(RAWINPUTHEADER));

		LPBYTE lpb = new BYTE[dwSize];
		if (lpb == NULL) {
			LOG_WARNING << "Invalid raw input device! HRAWINPUT "
				<< event.LParam;
			return;
		}

		if (GetRawInputData((HRAWINPUT)event.LParam, RID_INPUT, lpb, &dwSize,
			sizeof(RAWINPUTHEADER)) != dwSize)
		{
			LOG_WARNING << "GetRawInputData does not return correct size!"
				"HRAWINPUT " << lpb;
			return;
		}

		event.LParam = (long)lpb;
	}
#endif
}
