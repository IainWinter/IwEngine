#include "iw/input/ContextManager.h"
#include <iw\log\logger.h>

namespace IW {
	void ContextManager::CreateContext(
		unsigned int windowId,
		float width,
		float height)
	{
		m_contexts.emplace(windowId, width, height);
	}

	void ContextManager::HandleInput(
		InputEvent& input)
	{
		Context& context = m_contexts[input.WindowId];

		// Locks

		if (!!input.State) {
			switch (input.Name) {
				case CAPS_LOCK:   context.State.ToggleLock(CAPS_LOCK);   break;
				case NUM_LOCK:    context.State.ToggleLock(NUM_LOCK);    break;
				case SCROLL_LOCK: context.State.ToggleLock(SCROLL_LOCK); break;
			}
		}

		// States

		float lastState = context.State[input.Name];

		if (input.Name == MOUSEdX) {
			context.State[MOUSEX] += input.State;
		}

		else if (input.Name == MOUSEdY) {
			context.State[MOUSEY] += input.State;
		}

		else if (input.Name == MOUSEX) {
			context.State[MOUSEdX] = input.State - lastState;
			context.State[MOUSEX]  = input.State;
		}

		else if (input.Name == MOUSEY) {
			context.State[MOUSEdY] = input.State - lastState;
			context.State[MOUSEY]  = input.State;
		}

		else {
			context.State[input.Name] = input.State;
		}

		// Dispatch of callbacks

		if (input.Device == KEYBOARD) {
			if (input.State) {
				bool shifted = !!context.State[SHIFT];
				bool    caps = !!context.State.GetLock(CAPS_LOCK);

				char character = GetCharacter(input.Name, shifted, caps);

				if (character != '\0') {
					context.KeyTypedCallback(&context.State, input.Name, character);
				}
			}
			
			if (input.State != lastState) {
				context.KeyCallback(&context.State, input.Name, input.State);
			}
		}


		else if (input.Device == MOUSE) {
			if (input.Name == WHEEL) {
				context.MouseWheelCallback(&context.State, input.State);
			}

			else if (input.Name == MOUSEdX || input.Name == MOUSEdY
				  || input.Name == MOUSEX  || input.Name == MOUSEY)
			{
				context.MouseMovedCallback(&context.State,
					context.State[MOUSEX],  context.State[MOUSEY],
					context.State[MOUSEdX], context.State[MOUSEdY]);
			}

			else {
				context.MouseButtonCallback(&context.State, input.Name, input.State);
			}
		}
	}

	void ContextManager::SetMouseWheelCallback(
		unsigned int windowId,
		MouseWheelCallback callback)
	{
		m_contexts[windowId].MouseWheelCallback = callback;
	}

	void ContextManager::SetMouseMovedCallback(
		unsigned int windowId,
		MouseMovedCallback callback)
	{
		m_contexts[windowId].MouseMovedCallback = callback;
	}

	void ContextManager::SetMouseButtonCallback(
		unsigned int windowId,
		MouseButtonCallback callback)
	{
		m_contexts[windowId].MouseButtonCallback = callback;
	}

	void ContextManager::SetKeyCallback(
		unsigned int windowId,
		KeyCallback callback)
	{
		m_contexts[windowId].KeyCallback = callback;
	}

	void ContextManager::SetKeyTypedCallback(
		unsigned int windowId, 
		KeyTypedCallback callback)
	{
		m_contexts[windowId].KeyTypedCallback = callback;
	}
}
