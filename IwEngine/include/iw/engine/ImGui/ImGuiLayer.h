#pragma once

#include "iw/engine/Layer.h"

namespace IwEngine {
	class IWENGINE_API ImGuiLayer
		: public Layer
	{
	private:
		int counter = 0;
	public:
		ImGuiLayer();
		~ImGuiLayer();

		int Initialize() override;
		void Destroy() override;
		void ImGui() override;

		void Begin();
		void End();

		bool On(WindowResizedEvent& event);
		bool On(MouseMovedEvent&    event);
		bool On(MouseButtonEvent&   event);
	};
}
