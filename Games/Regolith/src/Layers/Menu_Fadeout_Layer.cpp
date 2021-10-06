#include "Layers/Menu_Fadeout_Layer.h"

int Menu_Fadeout_Layer::Initialize()
{
	m_background = m_screen->CreateElement(A_mesh_menu_background.MakeInstance());

	return 0;
}

void Menu_Fadeout_Layer::PostUpdate()
{
	m_screen->width  = Renderer->Width();
	m_screen->height = Renderer->Height();

	m_background->width  = m_screen->width;
	m_background->height = m_screen->height;

	m_timer += iw::DeltaTime();

	if (m_timer < m_time)
	{
		float blackness = m_timer / m_time;
		m_background->mesh.Material->Set("color", iw::Color(0, 0, 0, blackness));
	}
}
