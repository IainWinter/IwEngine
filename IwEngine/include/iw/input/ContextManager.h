#pragma once

#include "InputState.h"
#include "Context.h"
#include "Events/InputEvent.h"
#include "iw/util/set/sparse_set.h"

namespace IW {
inline namespace Input {
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
	};
}
}
