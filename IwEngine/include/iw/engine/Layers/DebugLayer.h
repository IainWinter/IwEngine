#pragma once

#include "iw/engine/Layer.h"

namespace IW {
namespace Engine {
	class DebugLayer
		: public Layer
	{
	private:
		std::vector<std::string> logs;

	public:
		IWENGINE_API
		DebugLayer();

		IWENGINE_API
		void PostUpdate() override;

		IWENGINE_API
		void ImGui() override;

		IWENGINE_API
		bool On(
			MouseWheelEvent& e) override;

		IWENGINE_API
		bool On(
			MouseMovedEvent& e) override;

		IWENGINE_API
		bool On(
			MouseButtonEvent& e) override;

		IWENGINE_API
		bool On(
			KeyEvent& e) override;

		IWENGINE_API
		bool On(
			KeyTypedEvent& e) override;

		IWENGINE_API
		bool On(
			WindowResizedEvent& e) override;
	};
}

	using namespace Engine;
}
