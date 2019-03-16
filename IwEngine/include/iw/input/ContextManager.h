#pragma once

#include "Events/InputEvent.h"
#include "iw/util/set/sparse_set.h"

namespace IwInput {
	struct Context {
		float               States[INPUT_COUNT];
		float               Width;
		float               Height;
		MouseWheelCallback  MouseWheelCallback;
		MouseMovedCallback  MouseMovedCallback;
		MouseButtonCallback MouseButtonCallback;
		KeyCallback         KeyCallback;

		Context(
			float width,
			float height)
			:Width(width)
			,Height(height)
		{
			memset(States, 0.0f, INPUT_COUNT * sizeof(float));
		}
	};

	class IWINPUT_API ContextManager {
	private:
		struct {
			iwutil::sparse_set<int, Context> m_contexts;
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

	};
}
