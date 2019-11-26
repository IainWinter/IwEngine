#pragma once

#include "iw/engine/Layer.h"

namespace IW {
inline namespace Engine {
	class IWENGINE_API DebugLayer
		: public Layer
	{
	private:
		std::vector<std::string> logs;

	public:
		DebugLayer();

		void PostUpdate() override;
		void ImGui() override;

		bool On(IW::MouseWheelEvent&    e) override;
		bool On(IW::MouseMovedEvent&    e) override;
		bool On(IW::MouseButtonEvent&   e) override;
		bool On(IW::KeyEvent&           e) override;
		bool On(IW::KeyTypedEvent&      e) override;
		bool On(IW::WindowResizedEvent& e) override;
	};
}
}
