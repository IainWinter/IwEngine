#include "Layers/Menu_Fadeout_Layer.h"

void Menu_Fadeout_Layer::UI()
{
	if (m_timer >= m_delay)
	{
		ImGui::GetForegroundDrawList()
			->AddRectFilled(
				ImVec2(bg_x,        bg_y), 
				ImVec2(bg_x + bg_w, bg_y + bg_h), 
				iw::Color(0, 0, 0, (m_timer - m_delay) / m_time).to32()
			);
	}

	m_timer += iw::RawDeltaTime();
}
