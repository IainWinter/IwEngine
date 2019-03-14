#include "iw/input/InputManager.h"

namespace IwInput {
	void InputManager::HandleEvent(
		int wid,
		int msg,
		short wParam,
		int lParam)
	{
		m_deviceManager.HandleEvent({ wid, msg, wParam, lParam });
	}

	void InputManager::HandleInput(
		InputEvent input)
	{
		m_callback(input); // temp
	}
}

