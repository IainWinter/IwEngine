#pragma once 

#include "iw/engine/Layer.h"
#include "iw/graphics/Font.h"

namespace IW {
	class ToolLayer
		: public Layer
	{
	private:
		Mesh* textMesh;
		iw::ref<Shader> fontShader;
		iw::ref<Font> font;

	public:
		ToolLayer();

		int Initialize() override;

		void PostUpdate() override;

		void ImGui() override;
	};
}
