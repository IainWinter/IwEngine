#include "Layers/Menu_Pause_Layer.h"

int Menu_Pause_Layer::Initialize()
{
	m_entity_screen = Space->CreateEntity<UIScreen>();

	m_screen = m_entity_screen.Set<UIScreen>();

	m_background  = m_screen->CreateElement(A_mesh_menu_background);
	m_pause_menu  = m_screen->CreateElement(A_mesh_menu_pause);
	m_pause_title = m_screen->CreateElement(A_mesh_menu_pause_title);

	return 0;
}

void Menu_Pause_Layer::Destroy()
{
	m_entity_screen.Destroy();
}

void Menu_Pause_Layer::OnPush()
{
	m_entity_screen.Revive();
}

void Menu_Pause_Layer::OnPop()
{
	m_entity_screen.Kill();
}

void Menu_Pause_Layer::PostUpdate()
{
	m_screen->width  = Renderer->Width();
	m_screen->height = Renderer->Height();

	m_background->width  = m_screen->width;
	m_background->height = m_screen->height;
	m_background->z = -5;

	m_pause_menu->height = 600;
	m_pause_menu->width  = 600;
	m_pause_menu->z = -6;

	m_pause_title->width  = m_pause_menu->width;
	m_pause_title->height = m_pause_menu->width;
	m_pause_title->x = m_pause_menu->x - m_pause_menu->width  + 15;
	m_pause_title->y = m_pause_menu->y + m_pause_menu->height - 5;
	m_pause_title->z = -7;
}
