#include "Layers/ToolLayer.h"
#include "imgui/imgui.h"

namespace IW {
	ToolLayer::ToolLayer()
		 : Layer("Tool")
	{}

	int ToolLayer::Initialize()
	{
		return 0;
	}

	void ToolLayer::ImGui() {
		ImGui::Begin("Toolbox");

		if (ImGui::Button("Create Cube")) {
			//Space->CreateEntity<Transform>();
		}

		ImGui::End();
	}
}

