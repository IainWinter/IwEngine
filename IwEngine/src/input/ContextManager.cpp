#include "iw/input/ContextManager.h"

namespace IwInput {
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
		if (input.Name == MOUSE_X_AXIS) {
			context.State[MOUSE_X_POS] += input.State;
		}

		else if (input.Name == MOUSE_Y_AXIS) {
			context.State[MOUSE_Y_POS] += input.State;
		}

		else if (input.Name == MOUSE_X_POS) {
			context.State[MOUSE_X_AXIS] = 
				input.State - context.State[MOUSE_X_POS];
		}

		else if (input.Name == MOUSE_Y_POS) {
			context.State[MOUSE_Y_AXIS] =
				input.State - context.State[MOUSE_Y_POS];
		}

		else if (input.Device == KEYBOARD) {
			// Need way more key interpretation this doesn't even work \/
			if (input.State) {
				context.KeyTypedCallback(context.State, input.Name, KeyTranslation[input.Name]);
			}
			
			if (input.State != context.State[input.Name]) {
				context.KeyCallback(context.State, input.Name, input.State);
			}
		}

		context.State[input.Name] = input.State;
		if (input.Device == MOUSE) {
			if (input.Name == MOUSE_WHEEL) {
				context.MouseWheelCallback(context.State, input.State);
			}

			else if (input.Name == MOUSE_Y_AXIS 
				 || input.Name == MOUSE_Y_POS)
			{
				context.MouseMovedCallback(context.State,
					context.State[MOUSE_X_POS],  context.State[MOUSE_Y_POS],
					context.State[MOUSE_X_AXIS], context.State[MOUSE_Y_AXIS]);
			}

			else if (input.Name != MOUSE_X_AXIS
				 && input.Name != MOUSE_X_POS)
			{
				context.MouseButtonCallback(context.State, input.Name, input.State);
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
