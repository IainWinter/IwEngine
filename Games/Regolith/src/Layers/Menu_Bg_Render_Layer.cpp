#include "Layers/Menu_Bg_Render_Layer.h"

void Menu_Bg_Render_Layer::UI()
{
	ImGui::SetNextWindowPos (ImVec2(bg_x, bg_y));
	ImGui::SetNextWindowSize(ImVec2(bg_w, bg_h));
	ImGui::Begin("BG Layer", nullptr, commonFlagsFocus);
	{
		ImGui::SetCursorPos(ImVec2(0, 0));
		ImGui::Image(Image("skybox_space.png"), ImVec2(bg_w, bg_h));

		ImGui::SetCursorPos(ImVec2(0, 0));
		ImGui::Image(Image("bg"), ImVec2(bg_w, bg_h), ImVec2(0, 1), ImVec2(1, 0));
	}
	ImGui::End();

	//ImGui::SetNextWindowFocus();
	//ImGui::ShowMetricsWindow();
}
