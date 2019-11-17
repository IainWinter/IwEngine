#pragma once

#include "InputState.h"
#include "Events/InputEvent.h"
#include "iw/util/set/sparse_set.h"

namespace IW {
inline namespace Input {
	struct Context {
		InputState          State;
		float               Width;
		float               Height;
		MouseWheelCallback  MouseWheelCallback;
		MouseMovedCallback  MouseMovedCallback;
		MouseButtonCallback MouseButtonCallback;
		KeyCallback         KeyCallback;
		KeyTypedCallback    KeyTypedCallback;

		Context(
			float width,
			float height)
			:Width(width)
			, Height(height)
		{}
	};

	class IWINPUT_API ContextManager {
	private:
		struct {
			iw::sparse_set<int, Context> m_contexts;
		};

	public:
		void CreateContext(
			unsigned int windowId,
			float width,
			float height);

		void HandleInput(
			InputEvent& input);

		void SetMouseWheelCallback(
			unsigned int windowId,
			MouseWheelCallback callback);

		void SetMouseMovedCallback(
			unsigned int windowId,
			MouseMovedCallback callback);

		void SetMouseButtonCallback(
			unsigned int windowId,
			MouseButtonCallback callback);

		void SetKeyCallback(
			unsigned int windowId,
			KeyCallback callback);

		void SetKeyTypedCallback(
			unsigned int windowId,
			KeyTypedCallback callback);
	};
}
}
