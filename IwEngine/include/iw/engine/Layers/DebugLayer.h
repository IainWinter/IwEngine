#pragma once

#include "iw/engine/Layer.h"

namespace iw {
namespace Engine {
	class DebugLayer
		: public Layer
	{
	private:
		std::vector<std::string> m_logs;
		std::unordered_map<std::string, std::tuple<float, float, std::vector<float>>> m_times;
		float m_smoothing;
		float m_removeOld;
		int m_skippedFrames;
		int m_frames;
		int m_eventCount;

	public:
		IWENGINE_API
		DebugLayer();

		void PrintTimes();
		void _enumerateTree(const iw::log_time& times);

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
