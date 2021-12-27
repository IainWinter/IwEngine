#include "Layers/Menu_Main_Layer.h"

int Menu_Main_Layer::Initialize()
{
	return 0;
}

void Menu_Main_Layer::UI()
{
	ImGui::GetIO().FontGlobalScale = bg_w / 800;

	ImGui::SetNextWindowPos(ImVec2(bg_x, bg_y));
	ImGui::SetNextWindowSize(ImVec2(bg_w, bg_h));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	ImGui::Begin("Main Menu", nullptr, commonFlags);
	{
		//ImGui::PushFont(font_regular);

		//ImGui::PopFont();
	}
	ImGui::End();

	ImGui::PopStyleVar(1);
}
