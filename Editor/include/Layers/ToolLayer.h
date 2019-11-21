#pragma once 

#include "iw/engine/Layer.h"

namespace IW {
	class ToolLayer
		: public Layer
	{
	public:
		ToolLayer();

		int Initialize() override;

		void PostUpdate() override;

		void ImGui() override;
	};
}
